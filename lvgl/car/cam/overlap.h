#ifndef OVERLAP_H
#define OVERLAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t circle_has_changed();
void reset_circle_change();
void update_circle(int32_t x, int32_t y, int32_t r);

void circle_move_x(int32_t v);
void circle_move_y(int32_t v);
void circle_move_r(int32_t v);

void get_circle_param(int32_t* x, int32_t* y, int32_t* r);
void save_circle_param();
void restore_circle_param();

#define SPOT_TL 0
#define SPOT_TR 1
#define SPOT_BL 2
#define SPOT_BR 3
#define NUM_SPOTS 4
void spot_move_x(int32_t spot_id, double v);
void spot_move_y(int32_t spot_id, double v);
void restore_spot_param();
void save_spot_param ();
void get_spot_locs(float* dst);

#define CAR_ICON_WIDTH  60
#define CAR_ICON_HEIGHT 103
void load_car_icon();

#ifdef __cplusplus
}
#endif

#endif