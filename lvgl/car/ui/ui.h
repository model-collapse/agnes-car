#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void back_view_eye_calib_screen(lv_obj_t* parent);
void back_view_dir_calib_screen(lv_obj_t* parent);
void dispose_all_from_scr(lv_obj_t* parent);
void reset_scene();
void back_view_down_calib_screen(lv_obj_t* parent);
void menu();
void update_video_ui();
void back_top_calib(lv_obj_t* parent);
void back_view_panel(lv_obj_t* parent);
void meters(lv_obj_t* parent);

void update_meters();

#ifdef __cplusplus
}
#endif

#endif
