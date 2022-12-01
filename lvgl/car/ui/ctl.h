#ifndef CTL_H
#define CTL_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void top_bar();

lv_obj_t* txt_btn(lv_obj_t* parent, const char* txt, int32_t width, int32_t height);
lv_obj_t* txt_btn_ck(lv_obj_t* parent, const char* txt, int32_t width, int32_t height);

#ifdef __cplusplus
}
#endif

#endif