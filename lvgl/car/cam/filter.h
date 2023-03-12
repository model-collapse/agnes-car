#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOOK_AT_PARAM_PATH "./look_at"
#define TOP_PARAM_PATH "./top"

#define TOP_EXP_MODE_VIEW 1
#define TOP_EXP_MODE_TRACK 2

int32_t lookat_has_changed();
void reset_lookat_change();

void set_lookat_model(int32_t model);

void move_look_vertical(double v);
void move_look_horizontal(double v);
void pan_look(double v);

void incr_fov(double v);
void restore_default_lookat_param();
void restore_lookat_param();
void save_lookat_param();

void save_top_param();
void restore_top_param();
void get_top_param(float* bw, float* bh, float* mg);
void set_top_param(float bw, float bh, float mg);
void update_top_view_matrix();
int32_t get_top_expand_mode();
void set_top_expand_mode(int32_t mode);

#ifdef __cplusplus
}
#endif

#endif