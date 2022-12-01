#ifndef CAM_H
#define CAM_H

#include <stdint.h>
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FRONT_CAM 1
#define BACK_CAM 2

void init_cams();
void release_cams();

bool camera_has_update(int32_t cam, bool reset_after_get);
bool camera_set_update(int32_t cam);

void lock_camera_update();
void unlock_camera_update();

void start_camera_update();
void end_camera_update();

#ifdef __cplusplus
}
#endif

#endif