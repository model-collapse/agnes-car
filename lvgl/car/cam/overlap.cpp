#include "overlap.h"
#include "overlap.hpp"
#include "model.h"
#include <stdio.h>

#define CIRCLE_PARAM_PATH "./circle"
#define SPOT_PARAM_PATH "./spots"

int32_t circle_x;
int32_t circle_y;
int32_t circle_r;
bool circle_changed;

void reset_circle_change() {
    circle_changed = false;
}

int32_t circle_has_changed() {
    return circle_changed ? 1 : 0;
}

void circle_move_x(int32_t v) {
    if (circle_x + v + circle_r > BACK_EYE_HEIGHT) {
        return;
    }

    if (circle_x + v - circle_r < 0) {
        return;
    }
    
    circle_x += v;
    circle_changed = true;
}

void circle_move_y(int32_t v) {
    if (circle_y + v + circle_r > BACK_EYE_HEIGHT) {
        return;
    }

    if (circle_y + v - circle_r < 0) {
        return;
    }
    
    circle_y += v;
    circle_changed = true;
}

void circle_move_r(int32_t v) 
{
    if (circle_x + v + circle_r > BACK_EYE_HEIGHT) {
        return;
    }

    if (circle_x - v - circle_r < 0) {
        return;
    }
    
    if (circle_y + v + circle_r > BACK_EYE_HEIGHT) {
        return;
    }

    if (circle_y - v - circle_r < 0) {
        return;
    }

    circle_r += v;
    circle_changed = true;
}

void update_circle(int32_t x, int32_t y, int32_t r) {
    circle_x = x;
    circle_y = y;
    circle_r = r;
    circle_changed = true;
}

void render_aim_circle(cv::Mat& frame, cv::Point center, int32_t radius) {
    cv::Scalar colorCircle1(255, 0, 54); // (B, G, R)
    cv::circle(frame, center, radius, colorCircle1, 1);
    cv::line(frame, cv::Point(center.x-radius, center.y), cv::Point(center.x+radius, center.y), colorCircle1, 1);
    cv::line(frame, cv::Point(center.x, center.y-radius), cv::Point(center.x, center.y+radius), colorCircle1, 1);

    /*for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
            frame.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 0, 54);
        }
    }*/
}

void render_circle(cv::Mat& frame) {
    //fprintf(stderr, "rendering circle...\n");
    render_aim_circle(frame, cv::Point(circle_x, circle_y), circle_r);
}

void get_circle_param(int32_t* x, int32_t* y, int32_t* r) {
    *x = circle_x;
    *y = circle_y;
    *r = circle_r;
}

void save_circle_param() {
    FILE* f = fopen(CIRCLE_PARAM_PATH, "w");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open circl configure file for write\n");
        return;
    }
    
    fprintf(f, "%d %d %d", circle_x, circle_y, circle_r);
    fprintf(stderr, "circle = [%d, %d, %d]\n", circle_x, circle_y, circle_r);
    fclose(f);
}

void restore_default_circle_param() {
    circle_x = 150;
    circle_y = 150;
    circle_r = 145;
}

void restore_circle_param() {
    FILE* f = fopen(CIRCLE_PARAM_PATH, "r");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open circl configure file\n");
        restore_default_circle_param();
        return;
    }

    fscanf(f, "%d %d %d", &circle_x, &circle_y, &circle_r);
    fprintf(stderr, "restored param: %d %d %d\n", circle_x, circle_y, circle_r);
    fclose(f);
}

void render_3x3_net(cv::Mat& frame) {
    int32_t xv = frame.cols / 3;
    int32_t yv = frame.rows / 3;

    cv::Scalar colorCircle1(255, 0, 54); // (B, G, R)
    cv::line(frame, cv::Point(xv, 0), cv::Point(xv, frame.rows), colorCircle1, 1);
    cv::line(frame, cv::Point(frame.cols - xv, 0), cv::Point(frame.cols - xv, frame.rows), colorCircle1, 1);
    cv::line(frame, cv::Point(0, yv), cv::Point(frame.cols, yv), colorCircle1, 1);
    cv::line(frame, cv::Point(0, frame.rows - yv), cv::Point(frame.cols, frame.rows - yv), colorCircle1, 1);
}

cv::Point2f spots[NUM_SPOTS] = {
    cv::Point2f(0.3, 0.3),
    cv::Point2f(0.7, 0.3),
    cv::Point2f(0.3, 0.7),
    cv::Point2f(0.7, 0.7)
};

cv::Vec3b spot_colors[NUM_SPOTS] = {
    cv::Vec3b(0x00, 0x5F, 0xFF),
    cv::Vec3b(0x00, 0xFF, 0x5F),
    cv::Vec3b(0xFF, 0x5F, 0x0F),
    cv::Vec3b(0xFF, 0x5F, 0xFF)
};

void spot_move_x(int32_t spot_id, double v) {
    spots[spot_id].x += v;
}

void spot_move_y(int32_t spot_id, double v) {
    spots[spot_id].y += v;
}

void save_spot_param () {
    FILE* f = fopen(SPOT_PARAM_PATH, "w");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open spot configure file for write\n");
        return;
    }

    fprintf(f, "%f,%f %f,%f %f,%f %f,%f", spots[0].x, spots[0].y, spots[1].x, spots[1].y, spots[2].x, spots[2].y, spots[3].x, spots[3].y);

    fclose(f);
}

void restore_spot_param() {
    FILE* f = fopen(SPOT_PARAM_PATH, "r");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open spot configure file for read\n");
        return;
    }

    fscanf(f, "%f,%f %f,%f %f,%f %f,%f", &spots[0].x, &spots[0].y, &spots[1].x, &spots[1].y, &spots[2].x, &spots[2].y, &spots[3].x, &spots[3].y);

    fclose(f);
}

void get_spot_locs(float* dst) {
    for (int32_t i = 0; i < NUM_SPOTS; i++) {
        dst[2 * i] = spots[i].x;
        dst[2 * i + 1] = spots[i].y;
    }
}

void render_calib_spots(cv::Mat& frame) {
    for (int i = 0; i < NUM_SPOTS; i++) {
        cv::Point p{(int)(spots[i].x * frame.cols), (int)(spots[i].y * frame.rows)};
        cv::Vec3b c = spot_colors[i];
        cv::line(frame, cv::Point(p.x - 5, p.y), cv::Point(p.x + 5, p.y), c, 1);
        cv::line(frame, cv::Point(p.x, p.y - 5), cv::Point(p.x, p.y + 5), c, 1);
    }
}

#define CAR_ICON_PATH "./car_icon.png"
cv::Mat car_icon;
void load_car_icon() {
    car_icon = cv::imread(CAR_ICON_PATH, cv::IMREAD_UNCHANGED);
    if (car_icon.empty()) {
        fprintf(stderr, "Fail to load car icon file!\n");
        abort();
    }

    cv::resize(car_icon, car_icon, cv::Size(CAR_ICON_WIDTH, CAR_ICON_HEIGHT), 0, 0, cv::INTER_LINEAR);
}

void render_car_icon(cv::Mat& frame) {
    int32_t offx = (frame.cols - car_icon.cols) / 2;
    int32_t offy = (frame.rows - car_icon.rows) / 5;

    cv::Mat sub = frame(cv::Rect(offx, offy, car_icon.cols, car_icon.rows));
    for (int32_t y = 0; y < sub.rows; y++) {
        for (int32_t x = 0; x < sub.cols; x++) {
            cv::Vec3b s = sub.at<cv::Vec3b>(y, x);
            cv::Vec4b cs = car_icon.at<cv::Vec4b>(y, x);
            float opa = cs[3] / 255.0;
            cv::Vec3f v = cv::Vec3f(s[0], s[1], s[2]) * (1 - opa) + cv::Vec3f(cs[0], cs[1], cs[2]) * opa;
            
            sub.at<cv::Vec3b>(y, x) = cv::Vec3b((uint8_t)v[0], (uint8_t)v[1], (uint8_t)v[2]);
        }
    }
}