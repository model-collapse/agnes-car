#ifndef NAV_H
#define NAV_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VELOCITY_BUF_RANGE CLOCKS_PER_SEC
#define OMEGA_BUF_RANGE CLOCKS_PER_SEC

struct nv_stat {
    float dir[3]; // north-east-above direction
    float omega[3]; // velocity of angles
    float v[3]; // velocity
};

int32_t init_nav_sensor();
int32_t reset_nav_sensor();
int32_t read_nav_stat(struct nv_stat* stat);
void sync_avgs();

#ifdef __cplusplus
}
#endif

#endif