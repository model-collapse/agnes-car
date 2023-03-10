#include "ui.h"
#include "lvgl/lvgl.h"
#include "ctl.h"
#include <stdio.h>
#include "lvgl/car/cam/models.h"

lv_obj_t* btn_calib_eye;
lv_obj_t* btn_calib_dir;
lv_obj_t* btn_calib_down;
lv_obj_t* btn_calib_top;
lv_obj_t* btn_view_panel;
lv_obj_t* btn_view_optf;
lv_obj_t* btn_meters;
lv_obj_t* btn_exit;

void menu_cb(lv_obj_t* obj, lv_event_t event);

void menu() {
    reset_scene();
    reset_all_models();

    lv_obj_t* parent = lv_scr_act();
    btn_calib_eye = txt_btn(parent, "Fisheye Calib.", 180, 50);
    lv_obj_align(btn_calib_eye, parent, LV_ALIGN_IN_TOP_LEFT, 40, 40);
    lv_obj_set_event_cb(btn_calib_eye, menu_cb);

    btn_calib_dir = txt_btn(parent, "Rev View Calib", 180, 50);
    lv_obj_align(btn_calib_dir, parent, LV_ALIGN_IN_TOP_LEFT, 40, 100);
    lv_obj_set_event_cb(btn_calib_dir, menu_cb);

    btn_calib_down = txt_btn(parent, "Down View Calib", 180, 50);
    lv_obj_align(btn_calib_down, parent, LV_ALIGN_IN_TOP_LEFT, 40, 160);
    lv_obj_set_event_cb(btn_calib_down, menu_cb);

    btn_calib_top = txt_btn(parent, "Top View Calib.", 180, 50);
    lv_obj_align(btn_calib_top, parent, LV_ALIGN_IN_TOP_LEFT, 40, 220);
    lv_obj_set_event_cb(btn_calib_top, menu_cb);

    btn_view_panel = txt_btn(parent, "Preview", 180, 50);
    lv_obj_align(btn_view_panel, parent, LV_ALIGN_IN_TOP_LEFT, 240, 40);
    lv_obj_set_event_cb(btn_view_panel, menu_cb);

    btn_view_optf = txt_btn(parent, "Optical flow", 180, 50);
    lv_obj_align(btn_view_optf, parent, LV_ALIGN_IN_TOP_LEFT, 240, 100);
    lv_obj_set_event_cb(btn_view_optf, menu_cb);

    btn_meters = txt_btn(parent, "Meters", 180, 50);
    lv_obj_align(btn_meters, parent, LV_ALIGN_IN_TOP_LEFT, 240, 160);
    lv_obj_set_event_cb(btn_meters, menu_cb);

    btn_exit = txt_btn(parent, "Exit", 180, 50);
    lv_obj_align(btn_exit, parent, LV_ALIGN_IN_TOP_LEFT, 240, 220);
    lv_obj_set_event_cb(btn_exit, menu_cb);

}

void menu_cb(lv_obj_t* obj, lv_event_t event){
    if (event == LV_EVENT_CLICKED) {
        reset_scene();
        lv_obj_t* parent = lv_scr_act();
        if (obj == btn_calib_eye) {
            back_view_eye_calib_screen(parent);
        } else if (obj == btn_calib_dir) {
            back_view_dir_calib_screen(parent);
        } else if (obj == btn_calib_down) {
            back_view_down_calib_screen(parent);
        } else if (obj == btn_calib_top) {
            back_top_calib(parent);
        } else if (obj == btn_view_panel) {
            back_view_panel(parent);
        } else if (obj == btn_view_optf) {
            optf_panel(parent);
        } else if (obj == btn_meters) {
            meters(parent);           
        } else if (obj == btn_exit) {
            abort();           
        }
    }
}