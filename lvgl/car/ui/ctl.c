#include "ctl.h"
#include "ui.h"
#include "../../lvgl.h"

void bar_cb(lv_obj_t* obj, lv_event_t event);

lv_obj_t* top_btn;
void top_bar() {
    lv_obj_t* scr = lv_scr_act();
    lv_obj_t* line = lv_line_create(scr, NULL);
    static lv_point_t line_points[] = {{4, 40}, {476, 37}};
    lv_line_set_points(line, line_points, 2);
    lv_obj_set_style_local_line_color(line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_line_width(line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_align(line, scr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    top_btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(top_btn, 480, 37);
    lv_obj_set_pos(top_btn, 0, 0);
    lv_obj_align(top_btn, scr, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_style_local_bg_opa(top_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_opa(top_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_OPA_COVER);
    lv_obj_set_style_local_outline_opa(top_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_obj_t* lb = lv_label_create(top_btn, NULL);
    lv_label_set_text(lb, "=");
    //lv_obj_set_style_local_text_font(lb, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_FONT_MONTSERRAT_24);
    lv_obj_set_style_local_radius(top_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(top_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(top_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(top_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_color(top_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));

    lv_obj_set_event_cb(top_btn, bar_cb);
}

void bar_cb(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        reset_scene();
        menu();
    }
}

lv_obj_t* txt_btn(lv_obj_t* parent, const char* txt, int32_t width, int32_t height) {
    lv_obj_t* btn = lv_btn_create(parent, NULL);
    lv_obj_set_size(btn, width, height);
    lv_obj_t* lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, txt);
    lv_obj_set_style_local_outline_opa(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_color(btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));

    return btn;
}

lv_obj_t* txt_btn_ck(lv_obj_t* parent, const char* txt, int32_t width, int32_t height) {
    lv_obj_t* btn = lv_btn_create(parent, NULL);
    lv_obj_set_size(btn, width, height);
    lv_obj_t* lbl = lv_label_create(btn, NULL);
    lv_label_set_text(lbl, txt);
    lv_obj_set_style_local_outline_opa(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_color(btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED | LV_BTN_STATE_CHECKED_RELEASED, LV_COLOR_MAKE(0xFF, 0x5F, 0x00));
    lv_btn_set_checkable(btn, true);

    return btn;
}