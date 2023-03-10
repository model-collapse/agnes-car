#include <stdio.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include "lvgl/car/cam/models.h"
#include "lvgl/car/cam/filter.hpp"
#include "lvgl/car/cam/filter.h"
#include "lvgl/car/cam/overlap.h"
#include <time.h>

int32_t main() {
    cv::Mat frame;
    cv::VideoCapture* back_cam = new cv::VideoCapture("/dev/video_cam_back", cv::CAP_V4L);
    if (!back_cam->isOpened()) {
        fprintf(stderr, "fail to open back cam!\n");
        abort();
    }

    restore_circle_param();
	restore_spot_param();
	restore_lookat_param();
	restore_top_param();
    set_lookat_model(MODEL_BACK_DIR);
    
    back_cam->set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    back_cam->set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    int32_t count = 0;

    fprintf(stderr, "testing FPS, will keep for 100s...\n");
    time_t start = time(NULL);
    time_t end = time(NULL) +  100;
    while (time(NULL) < end) {
        (*back_cam) >> frame;
        if (frame.empty()) {
            fprintf(stderr, "empty frame\n");
            break;
        }

        cv::Mat dst;
        extract_dir_view(frame, dst);

        count ++;
    }
    end = time(NULL);

    double FPS = (double)count / (end - start);
    fprintf(stderr, "FPS = %f\n", FPS);
    return 0;
}
