#ifndef MODEL_HPP
#define MODEL_HPP

#include <opencv4/opencv2/opencv.hpp>
#include <lvgl/lvgl.h>

#define MAX_FILTERS 4
#define MAX_OVERLAPS 4

typedef void (*ViewFilter) (const cv::Mat& src, cv::Mat& dst);
typedef void (*Overlap) (cv::Mat& img);
typedef void (*ModelInit) ();

struct ViewModel {
    int32_t id;
    int32_t cam;
    ModelInit init;
    ViewFilter filters[MAX_FILTERS];
    Overlap overlaps[MAX_OVERLAPS];
    int32_t width;
    int32_t height;

    //pthread_rwlock_t lock;
    lv_img_dsc_t desc;
    uint8_t* buf;

    bool enabled;
};

void init_model(ViewModel* m);
void cvtRGB16(const cv::Mat& smat, uint16_t* dst, int width, int height);

#endif