/**
 * @file evdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "evdev.h"
#if USE_EVDEV != 0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max);

/**********************
 *  STATIC VARIABLES
 **********************/
int evdev_fd;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

int evdev_key_val;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the evdev interface
 */
void evdev_init(void)
{
    evdev_fd = open(EVDEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return;
    }

    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_key_val = 0;
    evdev_button = LV_INDEV_STATE_REL;
}
/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_set_file(char* dev_name)
{ 
     if(evdev_fd != -1) {
        close(evdev_fd);
     }
     evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

     if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return false;
     }

     fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

     evdev_root_x = 0;
     evdev_root_y = 0;
     evdev_key_val = 0;
     evdev_button = LV_INDEV_STATE_REL;

     return true;
}

static void xpt2046_corr(int16_t * x, int16_t * y)
{
    //fprintf(stderr, "B X=%d, Y=%d\n", *x, *y);

#if XPT2046_XY_SWAP != 0
    int16_t swap_tmp;
    swap_tmp = *x;
    *x = *y;
    *y = swap_tmp;
#endif

    if((*x) > XPT2046_X_MIN)(*x) -= XPT2046_X_MIN;
    else(*x) = 0;

    if((*y) > XPT2046_Y_MIN)(*y) -= XPT2046_Y_MIN;
    else(*y) = 0;

    (*x) = (uint32_t)((uint32_t)(*x) * XPT2046_HOR_RES) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * XPT2046_VER_RES) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

#if XPT2046_X_INV != 0
    (*x) =  XPT2046_HOR_RES - (*x);
#endif

#if XPT2046_Y_INV != 0
    (*y) =  XPT2046_VER_RES - (*y);
#endif


}

/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    struct input_event in;

    while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
        if(in.type == EV_REL) {
            if(in.code == REL_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y += in.value;
				#else
					evdev_root_x += in.value;
				#endif
            else if(in.code == REL_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x += in.value;
				#else
					evdev_root_y += in.value;
				#endif
        } else if(in.type == EV_ABS) {
            if(in.code == ABS_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y = in.value;
				#else
					evdev_root_x = in.value;
				#endif
            else if(in.code == ABS_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x = in.value;
				#else
					evdev_root_y = in.value;
				#endif
            else if(in.code == ABS_MT_POSITION_X)
                                #if EVDEV_SWAP_AXES
                                        evdev_root_y = in.value;
                                #else
                                        evdev_root_x = in.value;
                                #endif
            else if(in.code == ABS_MT_POSITION_Y)
                                #if EVDEV_SWAP_AXES
                                        evdev_root_x = in.value;
                                #else
                                        evdev_root_y = in.value;
                                #endif
        } else if(in.type == EV_KEY) {
            if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                if(in.value == 0)
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 1)
                    evdev_button = LV_INDEV_STATE_PR;
            } else if(drv->type == LV_INDEV_TYPE_KEYPAD) {
		data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
		switch(in.code) {
			case KEY_BACKSPACE:
				data->key = LV_KEY_BACKSPACE;
				break;
			case KEY_ENTER:
				data->key = LV_KEY_ENTER;
				break;
			case KEY_UP:
				data->key = LV_KEY_UP;
				break;
			case KEY_LEFT:
				data->key = LV_KEY_PREV;
				break;
			case KEY_RIGHT:
				data->key = LV_KEY_NEXT;
				break;
			case KEY_DOWN:
				data->key = LV_KEY_DOWN;
				break;
			default:
				data->key = 0;
				break;
		}
		evdev_key_val = data->key;
		evdev_button = data->state;
		return false;
	    }
        }
    }

    if(drv->type == LV_INDEV_TYPE_KEYPAD) {
        /* No data retrieved */
        data->key = evdev_key_val;
	data->state = evdev_button;
	return false;
    }
    if(drv->type != LV_INDEV_TYPE_POINTER)
        return false;
    /*Store the collected data*/

    xpt2046_corr(&evdev_root_x, &evdev_root_y);

#if EVDEV_CALIBRATE
    data->point.x = map(evdev_root_x, EVDEV_HOR_MIN, EVDEV_HOR_MAX, 0, lv_disp_get_hor_res(drv->disp));
    data->point.y = map(evdev_root_y, EVDEV_VER_MIN, EVDEV_VER_MAX, 0, lv_disp_get_ver_res(drv->disp));
#else
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;
    //printf("X: %d, Y: %d\n", evdev_root_x, evdev_root_y);
#endif

    data->state = evdev_button;

    if(data->point.x < 0)
      data->point.x = 0;
    if(data->point.y < 0)
      data->point.y = 0;
    if(data->point.x >= lv_disp_get_hor_res(drv->disp))
      data->point.x = lv_disp_get_hor_res(drv->disp) - 1;
    if(data->point.y >= lv_disp_get_ver_res(drv->disp))
      data->point.y = lv_disp_get_ver_res(drv->disp) - 1;

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
