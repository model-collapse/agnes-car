#include "cam.h"
#include "models.h"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <pthread.h>

uint8_t cam_flag;
cv::VideoCapture* back_cam = NULL;

pthread_rwlock_t lock_back_cams;
pthread_rwlock_t lock_back_cam_updated;
bool back_cam_updated;
bool camera_has_update(int32_t cam, bool reset_after_get) {
    bool ret = false;
    if (cam == BACK_CAM) {
        if (reset_after_get) {
            pthread_rwlock_wrlock(&lock_back_cam_updated);
            ret = back_cam_updated;
            back_cam_updated = false;
            pthread_rwlock_unlock(&lock_back_cam_updated);
        } else {
            pthread_rwlock_rdlock(&lock_back_cam_updated);
            ret = back_cam_updated;
            pthread_rwlock_unlock(&lock_back_cam_updated);
        }
    }

    return ret;
}

bool camera_set_update(int32_t cam) {
    if (cam == BACK_CAM) {
        pthread_rwlock_wrlock(&lock_back_cam_updated);
        back_cam_updated = true;
        pthread_rwlock_unlock(&lock_back_cam_updated);
    }

    return true;
}

void init_cams() {
    back_cam = new cv::VideoCapture("/dev/video_cam_back", cv::CAP_V4L);
    if (!back_cam->isOpened()) {
        fprintf(stderr, "fail to open back cam!\n");
        abort();
    }

    back_cam->set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    back_cam->set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    pthread_rwlock_init(&lock_back_cam_updated, NULL);
    pthread_rwlock_init(&lock_back_cams, NULL);

    cv::Mat tFrame;
    (*back_cam) >> tFrame;

    cv::imwrite("frame.png", tFrame);
    return;
}

void release_cams() {
    if (back_cam != NULL) {
        back_cam->release();
        delete back_cam;
        back_cam = NULL;
    }
}

void lock_camera_update() {
    pthread_rwlock_rdlock(&lock_back_cams);
}

void unlock_camera_update() {
    pthread_rwlock_unlock(&lock_back_cams);
}

void start_camera_update() {
    pthread_rwlock_wrlock(&lock_back_cams);
}

void end_camera_update() {
    pthread_rwlock_unlock(&lock_back_cams);
}

