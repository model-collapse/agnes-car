#ifndef MODELS_H
#define MODELS_H

#include <stdint.h>
#include "model.h"
#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MODELS 10

void enable_model(int32_t id);
void disable_model(int32_t id);
void init_models();
void reset_all_models();

#ifdef __cplusplus
}
#endif

#endif