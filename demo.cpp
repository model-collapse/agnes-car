#include "lvgl/lvgl.h"
#include "lvgl/lv_drivers/display/fbdev.h"
#include "lvgl/lv_drivers/indev/evdev.h"
#include "lvgl/lv_examples/lv_examples.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "lvgl/car/cam/cam.h"
#include "lvgl/car/ui/ui.h"
//#include <stdio.h>
#include "lvgl/car/cam/models.h"
#include "lvgl/car/cam/overlap.h"
#include "lvgl/car/cam/filter.h"
#include "lvgl/car/sensor/nav.h"

// display buffer size - not sure if this size is really needed
#define LV_BUF_SIZE 384000		// 800x480
// A static variable to store the display buffers
static lv_disp_buf_t disp_buf;
// Static buffer(s). The second buffer is optional
static lv_color_t lvbuf1[LV_BUF_SIZE];
static lv_color_t lvbuf2[LV_BUF_SIZE];

void* camera_loop(void* p) {
	while(true) {
		update_frame(BACK_CAM);
		usleep(5e3);
	}

	return 0;
}

int main(void)
{
	// LittlevGL init
	lv_init();

	//Linux frame buffer device init
	fbdev_init();

	// Touch pointer device init
	evdev_init();

	// Initialize `disp_buf` with the display buffer(s)
	lv_disp_buf_init(&disp_buf, lvbuf1, lvbuf2, LV_BUF_SIZE);

	//lv_theme_apply(lv_scr_act(), LV_THEME_MATERIAL_FLAG_DARK);

	init_nav_sensor();
	init_cams();
	init_models();
	restore_circle_param();
	restore_spot_param();
	restore_lookat_param();
	restore_top_param();
	
	load_car_icon();

	// Initialize and register a display driver
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = fbdev_flush;	// flushes the internal graphical buffer to the frame buffer
	disp_drv.buffer = &disp_buf;		// set teh display buffere reference in the driver
	lv_disp_drv_register(&disp_drv);

	// Initialize and register a pointer device driver
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = evdev_read;    // defined in lv_drivers/indev/evdev.h
	lv_indev_drv_register(&indev_drv);

	sleep(1);
	//back_view_eye_calib_screen(lv_scr_act());
	//lv_demo_widgets();
	//back_view_dir_calib_screen(lv_scr_act());
	//back_view_eye_calib_screen(lv_scr_act());
	//back_view_down_calib_screen(lv_scr_act());
	//menu();
	//meters(lv_scr_act());
	back_view_panel(lv_scr_act());
	//back_top_calib(lv_scr_act());

	pthread_t cam_loop_thread;
	if (0 != pthread_create(&cam_loop_thread, NULL, camera_loop, NULL)) {
		fprintf(stderr, "[FATAL] fail to create cam loop thread!\n");
		return 0;
	}
	// Handle LitlevGL tasks (tickless mode)
	while(1) {
		if (camera_has_update(BACK_CAM, true)) {
			update_video_ui();
		}
		
		sync_avgs();
		update_meters();
		lock_camera_update();
		lv_tick_inc(5);
		lv_task_handler();
		unlock_camera_update();

		usleep(5000);
	}
	return 0;
}