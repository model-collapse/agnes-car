#include "ui.h"
#include "ctl.h"
#include <opencv2/opencv.hpp>
#include "lvgl/car/cam/model.hpp"

#define COMPASS_WIDTH 120
#define COMPASS_HEIGHT 120

extern lv_font_t meter_120;
lv_obj_t* lmeter;
lv_obj_t* lbl_speed;
lv_obj_t* img_compass;
lv_img_dsc_t compass_dsc;

cv::Mat compass_icon;
void meters(lv_obj_t* parent) {
    lmeter = lv_linemeter_create(parent, NULL);
    lv_linemeter_set_range(lmeter, 0, 100);                   /*Set the range*/
    lv_linemeter_set_value(lmeter, 10);                       /*Set the current value*/
    lv_linemeter_set_scale(lmeter, 240, 21);                  /*Set the angle and number of lines*/
    lv_obj_set_size(lmeter, 220, 220);
    lv_obj_align(lmeter, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 60);
    lv_obj_set_style_local_line_color(lmeter, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));

    lbl_speed = lv_label_create(lmeter, NULL);
    lv_obj_align(lbl_speed, lmeter, LV_ALIGN_CENTER, -50, -35);
    lv_obj_set_style_local_text_font(lbl_speed, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &meter_120);
    lv_label_set_text(lbl_speed, "29");

    lv_obj_t* lbl_unit = lv_label_create(lmeter, NULL);
    lv_obj_align(lbl_unit, lmeter, LV_ALIGN_CENTER, -10, 70);
    lv_label_set_text(lbl_unit, "km / h");

    compass_icon = cv::imread("./compass.png");
    compass_dsc.header.always_zero = 0;
    compass_dsc.header.w = COMPASS_WIDTH;
    compass_dsc.header.h = COMPASS_HEIGHT;
    compass_dsc.data_size = COMPASS_WIDTH*COMPASS_HEIGHT*sizeof(lv_color_t);
    compass_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    compass_dsc.data = (uint8_t*)malloc(compass_dsc.data_size);
    cvtRGB16(compass_icon, (uint16_t*)compass_dsc.data, COMPASS_WIDTH, COMPASS_HEIGHT);

    img_compass = lv_img_create(parent, NULL);
    lv_obj_set_size(img_compass, COMPASS_WIDTH, COMPASS_HEIGHT);
    lv_obj_align(img_compass, parent, LV_ALIGN_IN_TOP_LEFT, 300, 100);
    lv_img_set_src(img_compass, &compass_dsc);

    top_bar();
}

void momentum_meter(lv_obj_t* parent) {
    
}

void update_meters() {
    
}