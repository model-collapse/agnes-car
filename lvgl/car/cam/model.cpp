#include "model.h"
#include "models.h"
#include "cam.h"
#include "model.hpp"
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <pthread.h>

extern cv::VideoCapture* back_cam;
uint16_t toRGB16(const cv::Vec3b& v) {
    uint16_t b = v[0] >> 3;
    uint16_t g = v[1] >> 2;
    uint16_t r = v[2] >> 3;

    return (r << 11) | (g << 5) | b;
}

void cvtRGB16(const cv::Mat& smat, uint16_t* dst, int width, int height) {
    for (int i = 0; i < smat.rows; i++) {
        for (int j = 0; j < smat.cols; j++) {
            uint16_t c = toRGB16(smat.at<cv::Vec3b>(i,j));
            *dst = c;
            dst++;
        }
    }
}

extern ViewModel models[];
void update_frame(int32_t cam) {
    cv::Mat frame;
    if (cam == BACK_CAM) {
        if (!back_cam->isOpened()) {
            fprintf(stderr, "[WARN] the back cam is not opened\n");
            return;
        }

        (*back_cam) >> frame;   
    } else  {
        fprintf(stderr, "cam1\n");
        return;
    }

    for (int32_t i = 0; i < MAX_MODELS; i++) {
        clear_tagged_model_cache(models + i);
    }

    for (int32_t i = 0; i < MAX_MODELS; i++) {
        ViewModel* mp = models + i;
        if (!mp->enabled) {
            continue;
        }

        if (mp->id < 0) {
            break;;
        }
        
        if (frame.empty()) {
            fprintf(stderr, "ignoring empty frame\n");
            continue;
        }

        if (mp->cam != cam) {
            continue;
        }

        if (mp->init != NULL) {
            mp->init();
        }

        cv::Mat ftd = frame;
        for (int ii = 0; ii < MAX_FILTERS; ii++) {
            if (mp->filters[ii] != NULL) {
                cv::Mat mpb;
                //fprintf(stderr, "filter\n");
                mp->filters[ii](ftd, mpb, mp->filterCache[ii]);
                ftd = mpb;
            } else {
                break;
            }
        }

        cv::Mat rs;
        cv::resize(ftd, rs, cv::Size(mp->width, mp->height), 0, 0, cv::INTER_LINEAR);
        for (int ii = 0; ii < MAX_OVERLAPS; ii++) {
            if (mp->overlaps[ii] != NULL) {
                mp->overlaps[ii](rs);
            } else {
                break;
            }
        }

        start_camera_update();
        cvtRGB16(rs, (uint16_t*)mp->buf, mp->width, mp->height);
        end_camera_update();
    }

    camera_set_update(cam);
}

void init_model(ViewModel* m) {
    m->buf = (uint8_t*)malloc(sizeof(lv_color_t) * m->width * m->height);
    memset(m->buf, 0, (sizeof(lv_color_t) * m->width * m->height));
    //fprintf(stderr, "allocated on 0x%x\n", (uint32_t)m->buf);
    lv_img_dsc_t* dsc = &m->desc;
    dsc->header.always_zero = 0;
    dsc->header.w = m->width;
    dsc->header.h = m->height;
    dsc->data_size = m->width*m->height*sizeof(lv_color_t);
    dsc->header.cf = LV_IMG_CF_TRUE_COLOR;
    dsc->data = (const uint8_t *)m->buf;
}