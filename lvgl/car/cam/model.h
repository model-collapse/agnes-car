#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include "lvgl/lvgl.h"

#define MODEL_BACK_EYE 1
#define MODEL_BACK_TOP 2
#define MODEL_BACK_DIR 3
#define MODEL_BACK_DOWN 4
#define MODEL_BACK_TRACK 5
#define MODEL_BACK_OPTF 6

#define BACK_EYE_WIDTH 280
#define BACK_EYE_HEIGHT 280

#define BACK_DIR_WIDTH 280
#define BACK_DIR_HEIGHT 280

#define BACK_DOWN_WIDTH 280
#define BACK_DOWN_HEIGHT 280

#define BACK_TOP_WIDTH 180
#define BACK_TOP_HEIGHT 280

#ifdef __cplusplus
extern "C" {
#endif

const lv_img_dsc_t* image_dsc(int32_t model);
void update_frame(int32_t cam);

#ifdef __cplusplus
}
#endif

#endif