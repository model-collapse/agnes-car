#include "ui.h"
#include "ctl.h"
#include <opencv2/opencv.hpp>
#include "lvgl/car/cam/model.hpp"
#include "lvgl/car/sensor/nav.h"
#include <stdio.h>

#define COMPASS_WIDTH 120
#define COMPASS_HEIGHT 120

extern lv_font_t meter_110;
lv_obj_t* lmeter;
lv_obj_t* lbl_speed;
lv_obj_t* img_compass;
lv_img_dsc_t compass_dsc;

double l2LenP3f(float v[3]) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void momentum_meter(lv_obj_t* parent);

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
    lv_obj_align(lbl_speed, lmeter, LV_ALIGN_CENTER, -53, -35);
    lv_obj_set_style_local_text_font(lbl_speed, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &meter_110);
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

    momentum_meter(parent);

    top_bar();
}

#define METER_LINE_COUNT 9
#define METER_LINE_INTV 50
struct meter_bar_t {
    lv_obj_t* lines[METER_LINE_COUNT];
    lv_obj_t* labels[METER_LINE_COUNT];
    lv_obj_t* cursor;
    lv_obj_t* hor;
    lv_obj_t* lbl_anm;
    int y;
};

meter_bar_t lcur;
void momentum_meter(lv_obj_t* parent) {
    static lv_point_t ps[2] = {
        {0, 0},
        {0, 10}
    };
    
    for (int i = 0; i < METER_LINE_COUNT; i++) {
        int p = i - METER_LINE_COUNT / 2;
        lcur.lines[i] = lv_line_create(parent, NULL);
        lv_obj_align(lcur.lines[i], parent, LV_ALIGN_IN_TOP_LEFT, 240 + p * METER_LINE_INTV, 308);
        
        lv_line_set_points(lcur.lines[i], ps, 2);
        lv_obj_set_style_local_line_color(lcur.lines[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_set_style_local_line_width(lcur.lines[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    }

    static lv_point_t psq[2] = {
            {240, 300},
            {240, 320}
        };
    lcur.cursor = lv_line_create(parent, NULL);
    lv_line_set_points(lcur.cursor, psq, 2);
    //lv_obj_align(lcur.cursor, parent, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_local_line_color(lcur.cursor, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));
    lv_obj_set_style_local_line_width(lcur.cursor, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);

    static lv_point_t psc[2] = {
            {0, 318},
            {480, 318}
        };
    lcur.hor = lv_line_create(parent, NULL);
    lv_line_set_points(lcur.hor, psc, 2);
    lv_obj_set_style_local_line_color(lcur.hor, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_line_width(lcur.hor, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);

    lcur.lbl_anm = lv_label_create(parent, NULL);
    lv_label_set_text(lcur.lbl_anm, "0");
    lv_obj_align(lcur.lbl_anm, parent, LV_ALIGN_IN_TOP_LEFT, 220, 270);
    lv_obj_set_style_local_text_color(lcur.lbl_anm, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));
    lv_obj_set_style_local_text_font(lcur.lbl_anm, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);

    lv_obj_t* lbl_unit = lv_label_create(parent, NULL);
    lv_label_set_text(lbl_unit, "° / ms");
    lv_obj_align(lbl_unit, parent, LV_ALIGN_IN_TOP_LEFT, 245, 280);
}

#define MAX_SPEED 3.0
#define MAX_SPEED_LEN 20
char spbuf[MAX_SPEED_LEN];
void update_meters() {
    if (!img_compass) {
        return;
    }

    struct nv_stat stat;
    read_nav_stat(&stat);

    int compass_v = (int)(stat.dir[2] * 10);
    lv_img_set_angle(img_compass, (int16_t)compass_v);

    float speed = sqrt(l2LenP3f(stat.v));
    int32_t digit_a = ((int)speed) % 10;
    int32_t digit_b = ((int)(speed * 10)) % 10;

    //fprintf(stderr, "v = %f\n", speed);
    float speed_val = (speed / MAX_SPEED) * 100;
    //fprintf(stderr, "v = %f | %d\n", speed, (int)speed_val);
    lv_linemeter_set_value(lmeter, (int)(speed_val));

    snprintf(spbuf, MAX_SPEED_LEN, "%d.%d", digit_a, digit_b);
    lv_label_set_text(lbl_speed, spbuf);

    lv_obj_invalidate(img_compass);
    lv_obj_invalidate(lbl_speed);
}