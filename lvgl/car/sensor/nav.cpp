#include "nav.h"
#include <fcntl.h> 
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <opencv2/opencv.hpp>
#include <deque>
#include <pthread.h>
#include <unistd.h>

#define GRAV_COEFF 9.8

struct Status {
    cv::Vec3f data;
    clock_t last_update;
};

cv::Vec3f avg_velocity_buf[2];
cv::Vec3f avg_omega_buf[2];
cv::Vec3f* avg_velocity;
cv::Vec3f* avg_velocity_back;
void switch_avg_velocity() {
    cv::Vec3f* tmp = avg_velocity;
    avg_velocity = avg_velocity_back;
    avg_velocity_back = tmp;
}

cv::Vec3f* avg_omega;
cv::Vec3f* avg_omega_back;
void switch_avg_omega() {
    cv::Vec3f* tmp = avg_omega;
    avg_omega = avg_omega_back;
    avg_omega_back = tmp;
}

Status gdir_buf[2];
Status velocity_buf[2];
Status* gdir;
Status* gdir_back;
void switch_gdir() {
    Status* tmp = gdir;
    gdir = gdir_back;
    gdir_back = tmp;
}

Status* velocity;
Status* velocity_back;
void switch_velocity() {
    Status* tmp = velocity;
    velocity = velocity_back;
    velocity_back = tmp;
}

void update_velocity(const Status& st) {
    velocity_back->data = st.data;
    velocity_back->last_update = st.last_update;
    switch_velocity();
    velocity_back->data = st.data;
    velocity_back->last_update = st.last_update;
}

clock_t last_move;
void update_velocity_acc(const Status& racc) {
    cv::Vec3f acc = racc.data;
    acc[2] = 0;
    float diff = (float)(racc.last_update - velocity->last_update) / (float)CLOCKS_PER_SEC;
    cv::Vec3f data = velocity->data + acc * (diff);
    
    if (acc[0] != 0 || acc[1] != 0) {
        fprintf(stderr, "%f, %f, %f - %f, %f, %f | %f\n", acc[0], acc[1], acc[2], data[0], data[1], data[2], diff);
        last_move = clock();
    }
    Status s = {data, racc.last_update};
    if ((clock() - last_move) > CLOCKS_PER_SEC / 4) {
        s.data = cv::Vec3f(0, 0, 0);
    }   

    update_velocity(s);
}

void update_gdir(const Status& dir) {
    gdir_back->data = dir.data;
    gdir_back->last_update =dir.last_update;
    switch_gdir();
    gdir_back->data = dir.data;
    gdir_back->last_update =dir.last_update;
}

int32_t reset_nav_sensor() {
    clock_t now = clock();
    Status s = {cv::Vec3f(0, 0, 0), now};
    update_velocity(s);
}

std::deque<Status> velocity_q;
std::deque<Status> omega_q;
void truncate_buffers(clock_t now) {
    if (velocity_q.size() > 0) {
        while (velocity_q.front().last_update < now - VELOCITY_BUF_RANGE) {
            velocity_q.pop_front();
        }
    }

    if (omega_q.size() > 0) {
        while (omega_q.front().last_update < now - OMEGA_BUF_RANGE) {
            omega_q.pop_front();
        }
    }
}

cv::Vec3f calculate_avg_velocity() {
    cv::Vec3f sum(0, 0, 0);
    for (auto iter = velocity_q.begin(); iter != velocity_q.end(); iter++) {
        sum += iter->data;
    }

    return sum * (1.0 / velocity_q.size());
}

cv::Vec3f calculate_avg_omegav() {
    if (omega_q.size() == 0){
        return cv::Vec3f(0, 0, 0);
    }
    return (omega_q.back().data - omega_q.front().data) *(1000.0 / ((float)(omega_q.back().last_update - omega_q.front().last_update) / CLOCKS_PER_SEC));
}

cv::Vec3f regulate_acc_dir(cv::Vec3f acc, cv::Vec3f dir) {
    //fprintf(stderr, "%f, %f, %f\n", dir[0], dir[1], dir[2]);
    //fprintf(stderr, "%f, %f, %f\n", acc[0], acc[1], acc[2]);
    dir = dir * (CV_PI / 180.0);
    cv::Vec3f& theta = dir;
    cv::Mat R_x = (cv::Mat_<float>(3,3) << 1, 0, 0, 0, cos(theta[0]), -sin(theta[0]), 0, sin(theta[0]), cos(theta[0]) );
    cv::Mat R_y = (cv::Mat_<float>(3,3) << cos(theta[1]), 0, sin(theta[1]), 0, 1, 0, -sin(theta[1]), 0, cos(theta[1]) );
    cv::Mat R_z = (cv::Mat_<float>(3,3) << cos(theta[2]), -sin(theta[2]), 0, sin(theta[2]), cos(theta[2]), 0, 0, 0, 1);
    cv::Mat R = R_z * R_y * R_x;

    cv::Mat_<float> thetaM(3,1);
    thetaM << acc[0], acc[1], acc[2];

    cv::Mat m = R * thetaM;
    return cv::Vec3f(m.at<float>(0, 0), m.at<float>(0, 1), m.at<float>(0, 2));
}

cv::Vec3f trunc_acc(cv::Vec3f acc) {
    cv::Vec3f ret = acc;
    float s = 0;
    for (int32_t i = 0; i < 2; i++) {
        s += acc[i] * acc[i];
    }
    if (s < 0.04) {
        ret = cv::Vec3f(0, 0, 0);
    }
    
    return ret;
}

void update_status_rt(const Status& acc, const Status& dir) {
    //fprintf(stderr, ".");
    Status accr = acc;
    accr.data = trunc_acc(regulate_acc_dir(acc.data, dir.data));
    update_velocity_acc(accr);
    update_gdir(dir);
    //fprintf(stderr, "| %f\n", dir.data[2]);
}

void update_q() {
    clock_t now = clock();
    velocity_q.push_back(*velocity);
    omega_q.push_back(*gdir);
    truncate_buffers(now);
}

void init_buffers() {
    avg_velocity = avg_velocity_buf;
    avg_velocity_back = avg_velocity_buf + 1;

    avg_omega = avg_omega_buf;
    avg_omega_back = avg_omega_buf + 1;

    gdir = gdir_buf;
    gdir_back = gdir_buf + 1;

    velocity = velocity_buf;
    velocity_back = velocity_buf + 1;
}

void* sensor_read_loop(void*);
pthread_t sensor_loop_thread;
int fd;
int32_t init_nav_sensor() {
    init_buffers();

    fd = open("/dev/tty_po_sensor", O_RDWR | O_NOCTTY | O_SYNC);

    if (!fd) {
        fprintf(stderr, "Fail to open the sensor tty descriptor\n");
    }

    struct termios tty;
    if (tcgetattr (fd, &tty) != 0)
    {
        fprintf(stderr, "error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed (&tty, (speed_t)B115200);
    cfsetispeed (&tty, (speed_t)B115200);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    cfmakeraw(&tty);

    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error %d from tcsetattr\n!", errno);
    }

    reset_nav_sensor();
    if (0 != pthread_create(&sensor_loop_thread, NULL, sensor_read_loop, NULL)) {
        fprintf(stderr, "[FATAL] fail to create sensor loop thread!\n");
        abort();
    }
}

cv::Vec3f get_acc(char* buf) {
    uint8_t axl = buf[0];           
    uint8_t axh = buf[1];
    uint8_t ayl = buf[2];                                        
    uint8_t ayh = buf[3];
    uint8_t azl = buf[4];                                       
    uint8_t azh = buf[5];
    
    float k_acc = 16.0;
 
    float acc_x = (axh << 8 | axl) / 32768.0 * k_acc;
    float acc_y = (ayh << 8 | ayl) / 32768.0 * k_acc;
    float acc_z = (azh << 8 | azl) / 32768.0 * k_acc;
    if (acc_x >= k_acc) {
        acc_x -= 2 * k_acc;
    }
    if (acc_y >= k_acc) {
        acc_y -= 2 * k_acc;
    }
    if (acc_z >= k_acc) {
        acc_z-= 2 * k_acc;
    }
    
    return cv::Vec3f(acc_x,acc_y,acc_z);
}

cv::Vec3f get_angle(char* buf) {
    uint8_t rxl = buf[0];                                      
    uint8_t rxh = buf[1];
    uint8_t ryl = buf[2];                                        
    uint8_t ryh = buf[3];
    uint8_t rzl = buf[4];                                        
    uint8_t rzh = buf[5];
    float k_angle = 180.0;

    float angle_x = (rxh << 8 | rxl) / 32768.0 * k_angle;
    float angle_y = (ryh << 8 | ryl) / 32768.0 * k_angle;
    float angle_z = 270 - (rzh << 8 | rzl) / 32768.0 * k_angle;
    if (angle_x >= k_angle) {
        angle_x -= 2 * k_angle;
    }
    if (angle_y >= k_angle) {
        angle_y -= 2 * k_angle;
    }
    if (angle_z >=k_angle) {
        angle_z-= 2 * k_angle;
    }

    return cv::Vec3f(angle_x, angle_y, angle_z);
}

void print_hex(char* buf, int32_t n) {
    int i;
    for (i = 0; i < n; i++){
        printf("%02X", buf[i]);
    }
    printf("\n");
}

int32_t frame_state;
int32_t byte_num;
int32_t checksum;
bool sensor_reading = true;
#define STAT_WORD_SIZE 33
#define DATA_WORD_SIZE 8
void* sensor_read_loop(void*) {
    char acc_data[DATA_WORD_SIZE];
    char angle_data[DATA_WORD_SIZE];
    char gyro_data[DATA_WORD_SIZE];

    cv::Vec3f acc;
    cv::Vec3f angle;

    char buf[STAT_WORD_SIZE];
    while (sensor_reading) {
        memset(buf, 0, STAT_WORD_SIZE);
        int32_t n = read(fd, buf, STAT_WORD_SIZE);
        //print_hex(buf, STAT_WORD_SIZE);
        for (char* p = buf; p < buf + STAT_WORD_SIZE; p++) {
            if (frame_state == 0) {
                if (*p == 0x55 && byte_num == 0){ //0x55位于第一位时候，开始读取数据，增大bytenum
                    checksum=*p;
                    byte_num=1;
                    continue;
                } else if (*p == 0x51 and byte_num == 1) { //在byte不为0 且 识别到 0x51 的时候，改变frame
                    checksum += *p;
                    frame_state = 1;
                    byte_num = 2;
                } else if (*p == 0x52 and byte_num == 1) { //#同理
                    checksum += *p;
                    frame_state = 2;
                    byte_num = 2;
                } else if (*p == 0x53 and byte_num == 1) {
                    checksum += *p;
                    frame_state = 3;
                    byte_num = 2;
                }
            } else if (frame_state == 1) {
                if (byte_num < 10) {            // 读取8个数据
                    acc_data[byte_num-2] = *p; // 从0开始
                    checksum += *p;
                    byte_num += 1;
                } else {
                    if (*p == (checksum & 0xff)) { //  #假如校验位正确
                        acc = get_acc(acc_data);
                        //fprintf(stderr, "* %f, %f, %f\n", acc[0], acc[1], acc[2]);
                    }
                    checksum = 0;              //  #各数据归零，进行新的循环判断
                    byte_num = 0;
                    frame_state = 0;
                }
            } else if (frame_state == 2) {
                if (byte_num < 10) {
                    gyro_data[byte_num -2] = *p;
                    checksum += *p;
                    byte_num += 1;
                } else {
                    if (*p == (checksum & 0xff)) {
                        
                    }
                    checksum = 0;
                    byte_num = 0;
                    frame_state = 0;
                }
            } else if (frame_state == 3) {
                if (byte_num < 10) {
                    angle_data[byte_num - 2] = *p;
                    checksum += *p;
                    byte_num += 1;
                } else {
                    if (*p == (checksum & 0xff)) {
                        angle = get_angle(angle_data);
                        clock_t now = clock();
                        Status accs = {acc, now};
                        Status dirs = {angle, now};
                        update_status_rt(accs, dirs);
                    }
                    checksum = 0;
                    byte_num = 0;
                    frame_state = 0;
                    acc = cv::Vec3f(0, 0, 0);
                    angle = cv::Vec3f(0, 0, 0);
                }
            }
        }
    }
}

void sync_avgs() {
    cv::Vec3f v = calculate_avg_omegav();
    *avg_omega_back = v;
    switch_avg_omega();
    *avg_omega_back = v;

    v = calculate_avg_velocity();
    *avg_velocity_back = v;
    switch_avg_velocity();
    *avg_velocity_back = v;
}

int32_t read_nav_stat(struct nv_stat* stat) {
    update_q();

    cv::Vec3f* avgv = avg_velocity;
    cv::Vec3f* omgb = avg_omega;
    cv::Vec3f* dr = &(gdir->data);

    for (int i = 0; i < 3; i++) {
        stat->v[i] = (*avgv)[i] * GRAV_COEFF * 0.277777;
        stat->omega[i] = (*omgb)[i];
        stat->dir[i] = (*dr)[i];
    }

    //fprintf(stderr, "* %f, %f, %f\n", stat->v[0], stat->v[1], stat->v[2]);

    return 0;
}