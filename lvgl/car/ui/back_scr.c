#include "ui.h"
#include "lvgl/src/lv_misc/lv_ll.h"
#include "lvgl/car/cam/cam.h"
#include "lvgl/car/cam/models.h"
#include <stdio.h>
#include "ctl.h"
#include "lvgl/car/cam/overlap.h"
#include "lvgl/car/cam/filter.h"

#define MAX_LBL_LEN 100

lv_obj_t* img_eye;
lv_obj_t* img_top;
const lv_img_dsc_t* img_eye_dsc;

lv_obj_t* up_btn;
lv_obj_t* left_btn;
lv_obj_t* right_btn;
lv_obj_t* down_btn;
lv_obj_t* radius_up_btn;
lv_obj_t* radius_down_btn;
lv_obj_t* save_btn;

lv_obj_t* rleft_btn;
lv_obj_t* rright_btn;

const int32_t car_width = 68;
const int32_t car_length = 122;

void eye_calib_event_cb(lv_obj_t * obj, lv_event_t event);
void dir_calib_event_cb(lv_obj_t* obj, lv_event_t event);
void down_calib_event_cb(lv_obj_t* obj, lv_event_t event);
void top_view_calib_event_cb(lv_obj_t* obj, lv_event_t event);

void back_view_screen(lv_obj_t* parent) {
    //reset_all_models();
    //enable_model(MODEL_BACK_CAM_EYE);
    lv_obj_set_style_local_bg_color(parent, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    img_eye_dsc = image_dsc(MODEL_BACK_EYE);

    img_eye = lv_img_create(parent, NULL);
    lv_obj_align(img_eye, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_eye, 0, 40);
    lv_obj_set_size(img_eye, BACK_EYE_WIDTH, BACK_EYE_HEIGHT);
    lv_img_set_src(img_eye, img_eye_dsc);

}

void back_view_eye_calib_screen(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_EYE);

    img_eye_dsc = image_dsc(MODEL_BACK_EYE);
    img_eye = lv_img_create(parent, NULL);

    lv_obj_align(img_eye, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_eye, 0, 40);
    lv_obj_set_size(img_eye, BACK_EYE_WIDTH, BACK_EYE_HEIGHT);
    
    lv_img_set_src(img_eye, img_eye_dsc);

    up_btn = txt_btn(parent, LV_SYMBOL_UP, 60, 40);
    lv_obj_align(up_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(up_btn, 360, 50);
    lv_obj_set_event_cb(up_btn, eye_calib_event_cb);

    left_btn = txt_btn(parent, LV_SYMBOL_LEFT, 60, 40);
    lv_obj_align(left_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(left_btn, 320, 100);
    lv_obj_set_event_cb(left_btn, eye_calib_event_cb);

    right_btn = txt_btn(parent, LV_SYMBOL_RIGHT, 60, 40);
    lv_obj_align(right_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(right_btn, 400, 100);
    lv_obj_set_event_cb(right_btn, eye_calib_event_cb);

    down_btn = txt_btn(parent, LV_SYMBOL_DOWN, 60, 40);
    lv_obj_align(down_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(down_btn, 360, 150);
    lv_obj_set_event_cb(down_btn, eye_calib_event_cb);

    radius_up_btn = txt_btn(parent, LV_SYMBOL_UP, 60, 40);
    lv_obj_align(radius_up_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(radius_up_btn, 340, 200);
    lv_obj_set_event_cb(radius_up_btn, eye_calib_event_cb);
    
    radius_down_btn = txt_btn(parent, LV_SYMBOL_DOWN, 60, 40);
    lv_obj_align(radius_down_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(radius_down_btn, 410, 200);
    lv_obj_set_event_cb(radius_down_btn, eye_calib_event_cb);

    save_btn = txt_btn(parent, "Save", 100, 40);
    lv_obj_align(save_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(save_btn, 350, 260);
    lv_obj_set_event_cb(save_btn, eye_calib_event_cb);

    lv_obj_t* rlbl = lv_label_create(parent, NULL);
    lv_label_set_text(rlbl, "R:");
    lv_obj_set_pos(rlbl, 310, 210);

    top_bar();
}

lv_obj_t* img_dir;
const lv_img_dsc_t* img_dir_dsc;
void back_view_dir_calib_screen(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_DIR);

    img_dir_dsc = image_dsc(MODEL_BACK_DIR);

    img_dir = lv_img_create(parent, NULL);
    lv_obj_align(img_dir, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_dir, 0, 40);
    lv_obj_set_size(img_dir, BACK_EYE_WIDTH, BACK_EYE_HEIGHT);
    lv_img_set_src(img_dir, img_dir_dsc);

    up_btn = txt_btn(parent, LV_SYMBOL_UP, 50, 60);
    lv_obj_align(up_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(up_btn, 360, 50);
    lv_obj_set_event_cb(up_btn, dir_calib_event_cb);

    left_btn = txt_btn(parent, LV_SYMBOL_LEFT, 50, 60);
    lv_obj_align(left_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(left_btn, 300, 120);
    lv_obj_set_event_cb(left_btn, dir_calib_event_cb);

    right_btn = txt_btn(parent, LV_SYMBOL_RIGHT, 50, 60);
    lv_obj_align(right_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(right_btn, 420, 120);
    lv_obj_set_event_cb(right_btn, dir_calib_event_cb);

    down_btn = txt_btn(parent, LV_SYMBOL_DOWN, 50, 60);
    lv_obj_align(down_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(down_btn, 360, 120);
    lv_obj_set_event_cb(down_btn, dir_calib_event_cb);

    rleft_btn = txt_btn(parent, LV_SYMBOL_PREV, 50, 60);
    lv_obj_align(rleft_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(rleft_btn, 300, 50);
    lv_obj_set_event_cb(rleft_btn, dir_calib_event_cb);

    rright_btn = txt_btn(parent, LV_SYMBOL_NEXT, 50, 60);
    lv_obj_align(rright_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(rright_btn, 420, 50);
    lv_obj_set_event_cb(rright_btn, dir_calib_event_cb);

    radius_up_btn = txt_btn(parent, LV_SYMBOL_UP, 60, 40);
    lv_obj_align(radius_up_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(radius_up_btn, 340, 200);
    lv_obj_set_event_cb(radius_up_btn, dir_calib_event_cb);
    
    radius_down_btn = txt_btn(parent, LV_SYMBOL_DOWN, 60, 40);
    lv_obj_align(radius_down_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(radius_down_btn, 410, 200);
    lv_obj_set_event_cb(radius_down_btn, dir_calib_event_cb);

    save_btn = txt_btn(parent, "Save", 100, 40);
    lv_obj_align(save_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(save_btn, 350, 260);
    lv_obj_set_event_cb(save_btn, dir_calib_event_cb);

    lv_obj_t* rlbl = lv_label_create(parent, NULL);
    lv_label_set_text(rlbl, "FOV:");
    lv_obj_set_pos(rlbl, 290, 210);

    top_bar();
}

lv_obj_t* img_down;
const lv_img_dsc_t* img_down_dsc;

lv_obj_t* ck_look;
lv_obj_t* ck_top_left;
lv_obj_t* ck_top_right;
lv_obj_t* ck_bot_left;
lv_obj_t* ck_bot_right;
lv_obj_t* ck_bot_view;

int32_t spot_state;
void back_view_down_calib_screen(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_DOWN);
    
    img_down_dsc = image_dsc(MODEL_BACK_DOWN);
    img_down = lv_img_create(parent, NULL);
    lv_obj_align(img_down, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_down, 0, 40);
    lv_obj_set_size(img_down, BACK_DOWN_WIDTH, BACK_DOWN_HEIGHT);
    lv_img_set_src(img_down, img_down_dsc);

    up_btn = txt_btn(parent, LV_SYMBOL_UP, 50, 60);
    lv_obj_align(up_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(up_btn, 360, 50);
    lv_obj_set_event_cb(up_btn, down_calib_event_cb);

    left_btn = txt_btn(parent, LV_SYMBOL_LEFT, 50, 60);
    lv_obj_align(left_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(left_btn, 300, 120);
    lv_obj_set_event_cb(left_btn, down_calib_event_cb);

    right_btn = txt_btn(parent, LV_SYMBOL_RIGHT, 50, 60);
    lv_obj_align(right_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(right_btn, 420, 120);
    lv_obj_set_event_cb(right_btn, down_calib_event_cb);

    down_btn = txt_btn(parent, LV_SYMBOL_DOWN, 50, 60);
    lv_obj_align(down_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(down_btn, 360, 120);
    lv_obj_set_event_cb(down_btn, down_calib_event_cb);

    rleft_btn = txt_btn(parent, LV_SYMBOL_PREV, 50, 60);
    lv_obj_align(rleft_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(rleft_btn, 300, 50);
    lv_obj_set_event_cb(rleft_btn, down_calib_event_cb);

    rright_btn = txt_btn(parent, LV_SYMBOL_NEXT, 50, 60);
    lv_obj_align(rright_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(rright_btn, 420, 50);
    lv_obj_set_event_cb(rright_btn, down_calib_event_cb);

    ck_top_left = txt_btn_ck(parent, "TL", 50, 35);
    lv_obj_align(ck_top_left, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(ck_top_left, 360, 190);
    lv_obj_set_event_cb(ck_top_left, down_calib_event_cb);

    ck_top_right = txt_btn_ck(parent, "TR", 50, 35);
    lv_obj_align(ck_top_right, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(ck_top_right, 420, 190);
    lv_obj_set_event_cb(ck_top_right, down_calib_event_cb);

    ck_bot_left = txt_btn_ck(parent, "BL", 50, 35);
    lv_obj_align(ck_bot_left, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(ck_bot_left, 360, 230);
    lv_obj_set_event_cb(ck_bot_left, down_calib_event_cb);

    ck_bot_right = txt_btn_ck(parent, "BR", 50, 35);
    lv_obj_align(ck_bot_right, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(ck_bot_right, 420, 230);
    lv_obj_set_event_cb(ck_bot_right, down_calib_event_cb);

    ck_bot_view = txt_btn_ck(parent, "VW", 50, 50);
    lv_obj_align(ck_bot_view, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(ck_bot_view, 300, 200);
    lv_obj_set_event_cb(ck_bot_view, down_calib_event_cb);
    lv_btn_set_state(ck_bot_view, LV_BTN_STATE_CHECKED_RELEASED);
    spot_state = -1;

    save_btn = txt_btn(parent, "Save", 100, 35);
    lv_obj_align(save_btn, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(save_btn, 350, 280);
    lv_obj_set_event_cb(save_btn, down_calib_event_cb);

    top_bar();
}

lv_obj_t* img_top;
const lv_img_dsc_t* img_top_dsc;

lv_obj_t* slider_box_width;
lv_obj_t* slider_box_height;
lv_obj_t* slider_margin;

lv_obj_t* lbl_bwidth;
lv_obj_t* lbl_bheight;
lv_obj_t* lbl_bmargin;

void back_top_calib(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_TOP);

    img_top_dsc = image_dsc(MODEL_BACK_TOP);
    img_top = lv_img_create(parent, NULL);
    lv_obj_align(img_top, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_top, 0, 40);
    lv_obj_set_size(img_top, BACK_TOP_WIDTH, BACK_TOP_HEIGHT);
    lv_img_set_src(img_top, img_top_dsc);

    lv_obj_t* lbl_width_calib = lv_label_create(parent, NULL);
    lv_label_set_text(lbl_width_calib, "Box Width:");
    lv_obj_set_size(lbl_width_calib, 210, 30);
    lv_obj_align(lbl_width_calib, parent, LV_ALIGN_IN_TOP_LEFT, 210, 50);

    slider_box_width = lv_slider_create(parent, NULL);
    lv_obj_align(slider_box_width, parent, LV_ALIGN_IN_TOP_LEFT, 210, 80);
    lv_obj_set_size(slider_box_width, 240, 20);
    lv_slider_set_range(slider_box_width, 10, 100);
    lv_obj_set_event_cb(slider_box_width, top_view_calib_event_cb);

    lv_obj_t* lbl_height_calib = lv_label_create(parent, NULL);
    lv_label_set_text(lbl_height_calib, "Box Height:");
    lv_obj_set_size(lbl_height_calib, 210, 30);
    lv_obj_align(lbl_height_calib, parent, LV_ALIGN_IN_TOP_LEFT, 210, 120);

    slider_box_height = lv_slider_create(parent, NULL);
    lv_obj_align(slider_box_height, parent, LV_ALIGN_IN_TOP_LEFT, 210, 150);
    lv_obj_set_size(slider_box_height, 240, 20);
    lv_slider_set_range(slider_box_height, 10, 100);
    lv_obj_set_event_cb(slider_box_height, top_view_calib_event_cb);

    lv_obj_t* lbl_width_car = lv_label_create(parent, NULL);
    lv_label_set_text(lbl_width_car, "Margin:");
    lv_obj_set_size(lbl_width_car, 210, 30);
    lv_obj_align(lbl_width_car, parent, LV_ALIGN_IN_TOP_LEFT, 210, 190);

    slider_margin = lv_slider_create(parent, NULL);
    lv_obj_align(slider_margin, parent, LV_ALIGN_IN_TOP_LEFT, 210, 220);
    lv_obj_set_size(slider_margin, 240, 20);
    lv_slider_set_range(slider_margin, 0, 50);
    lv_obj_set_event_cb(slider_margin, top_view_calib_event_cb);

    save_btn = txt_btn(parent, "Save", 80, 40);
    lv_obj_align(save_btn, parent, LV_ALIGN_IN_TOP_LEFT, 370, 270);
    lv_obj_set_event_cb(save_btn, top_view_calib_event_cb);

    char buf[MAX_LBL_LEN];
    snprintf(buf, MAX_LBL_LEN, "Car Width: %dcm\nCar Length: %dcm", car_width, car_length);
    lv_obj_t* lbl_car_info = lv_label_create(parent, NULL);
    lv_obj_set_size(lbl_car_info, 300, 30);
    lv_obj_align(lbl_car_info, parent, LV_ALIGN_IN_TOP_LEFT, 190, 270);
    lv_label_set_text(lbl_car_info, buf);

    lbl_bwidth = lv_label_create(parent, NULL);
    lv_obj_set_size(lbl_bwidth, 100, 30);
    lv_obj_align(lbl_bwidth, parent, LV_ALIGN_IN_TOP_LEFT, 380, 50);
    lv_label_set_text(lbl_bwidth, "10cm");

    lbl_bheight = lv_label_create(parent, NULL);
    lv_obj_set_size(lbl_bheight, 100, 30);
    lv_obj_align(lbl_bheight, parent, LV_ALIGN_IN_TOP_LEFT, 380, 120);
    lv_label_set_text(lbl_bheight, "10cm");

    lbl_bmargin = lv_label_create(parent, NULL);
    lv_obj_set_size(lbl_bmargin, 100, 30);
    lv_obj_align(lbl_bmargin, parent, LV_ALIGN_IN_TOP_LEFT, 380, 190);
    lv_label_set_text(lbl_bmargin, "10cm");

    float bw, bh, mg;
    get_top_param(&bw, &bh, &mg);
    lv_slider_set_value(slider_box_width, bw, LV_ANIM_OFF);
    lv_slider_set_value(slider_box_height, bh, LV_ANIM_OFF);
    lv_slider_set_value(slider_margin, mg, LV_ANIM_OFF);
    update_top_view_matrix();

    top_bar();
}

void back_view_panel(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_TOP);
    enable_model(MODEL_BACK_DIR);

    img_top_dsc = image_dsc(MODEL_BACK_TOP);
    img_top = lv_img_create(parent, NULL);
    lv_obj_align(img_top, parent, LV_ALIGN_IN_TOP_LEFT, 300, 40);
    lv_obj_set_size(img_top, BACK_TOP_WIDTH, BACK_TOP_HEIGHT);
    lv_img_set_src(img_top, img_top_dsc);

    img_dir_dsc = image_dsc(MODEL_BACK_DIR);
    img_dir = lv_img_create(parent, NULL);
    lv_obj_align(img_dir, parent, LV_ALIGN_IN_TOP_LEFT, 0, 40);
    lv_obj_set_size(img_top, BACK_DIR_WIDTH, BACK_DIR_HEIGHT);
    lv_img_set_src(img_dir, img_dir_dsc);

    top_bar();
}

void optf_panel(lv_obj_t* parent) {
    reset_all_models();
    enable_model(MODEL_BACK_OPTF);
    
    img_top_dsc = image_dsc(MODEL_BACK_OPTF);
    img_top = lv_img_create(parent, NULL);
    lv_obj_align(img_top, parent, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(img_top, 0, 40);
    lv_obj_set_size(img_top, BACK_TOP_WIDTH, BACK_TOP_HEIGHT);
    lv_img_set_src(img_top, img_top_dsc);

    top_bar();
}

void update_video_ui() {
    if (img_eye != NULL) {
        lv_obj_invalidate(img_eye);
    }

    if (img_top != NULL) {
        lv_obj_invalidate(img_top);
    }

    if (img_dir != NULL) {
        lv_obj_invalidate(img_dir);
    }

    if (img_down != NULL) {
        lv_obj_invalidate(img_down);
    }
}

extern lv_obj_t* img_compass;
void reset_scene() {
    dispose_all_from_scr(lv_scr_act());
    img_eye = NULL;
    img_top = NULL;
    img_dir = NULL;
    img_down = NULL;
    img_compass = NULL;
}

void dispose_all_from_scr(lv_obj_t* parent) {
    lv_obj_clean(parent);
    
    /*lv_obj_t ** obj;
    _LV_LL_READ(parent->child_ll, obj) {
        fprintf(stderr, "deleting, ");
        lv_obj_del(*obj);
    }*/
}

#define PI 3.14159265359
void dir_calib_event_cb(lv_obj_t* obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_PRESSED:
            if (obj == up_btn) {
                move_look_vertical(-0.05);
            } else if (obj == down_btn) {
                move_look_vertical(0.05);
            } else if (obj == left_btn) {
                move_look_horizontal(-0.05);
            } else if (obj == right_btn) {
                move_look_horizontal(0.05);
            } else if (obj == radius_up_btn) {
                incr_fov(1);
            } else if (obj == radius_down_btn) {
                incr_fov(-1);
            } else if (obj == rleft_btn) {
                pan_look(-PI / 20);
            } else if (obj == rright_btn) {
                pan_look(PI / 20);
            }
            break;

        case LV_EVENT_CLICKED:
            if (obj == save_btn) {
                save_lookat_param();
            }
    }
}

void eye_calib_event_cb(lv_obj_t* obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_PRESSED:
            if (obj == up_btn) {
                circle_move_y(-1);
            } else if (obj == down_btn) {
                circle_move_y(1);
            } else if (obj == left_btn) {
                circle_move_x(-1);
            } else if (obj == right_btn) {
                circle_move_x(1);
            } else if (obj == radius_up_btn) {
                circle_move_r(1);
            } else if (obj == radius_down_btn) {
                circle_move_r(-1);
            }
            break;

        case LV_EVENT_SHORT_CLICKED:
            //printf("Short clicked\n");
            break;

        case LV_EVENT_CLICKED:
            if (obj == save_btn) {
                save_circle_param();
            }

            break;

        case LV_EVENT_LONG_PRESSED:
            //printf("Long press\n");
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            //printf("Long press repeat\n");
            break;

        case LV_EVENT_RELEASED:
            //printf("Released\n");
            break;
    }

       /*Etc.*/
}

bool in_check_list(lv_obj_t** ck_group, lv_obj_t* obj, int32_t size) {
    for (int i = 0; i < size; i++) {
        if (ck_group[i] == obj) {
            return true;
        }
    }

    return false;
}

void deal_check(lv_obj_t** ck_group, lv_obj_t* obj, int32_t size) {
    for (int i = 0; i < size; i++) {
        if (ck_group[i] != obj) {
            lv_btn_set_state(ck_group[i], LV_STATE_DEFAULT);
        }
    }

    if (obj == ck_top_left) {
        spot_state = SPOT_TL;
    } else if (obj == ck_top_right) {
        spot_state = SPOT_TR;
    } else if (obj == ck_bot_left) {
        spot_state = SPOT_BL;
    } else if (obj == ck_bot_right) {
        spot_state = SPOT_BR;
    } else if (obj == ck_bot_view) {
        spot_state = -1;
    }
}

void down_calib_event_cb(lv_obj_t* obj, lv_event_t event){
    lv_obj_t* ck_group[] = {ck_bot_left, ck_bot_right, ck_top_left, ck_top_right, ck_bot_view};

    switch(event) {
        case LV_EVENT_PRESSED:
            if (spot_state < 0) {
                if (obj == up_btn) {
                    move_look_vertical(-0.05);
                } else if (obj == down_btn) {
                    move_look_vertical(0.05);
                } else if (obj == left_btn) {
                    move_look_horizontal(-0.05);
                } else if (obj == right_btn) {
                    move_look_horizontal(0.05);
                } else if (obj == rleft_btn) {
                    pan_look(-PI / 20);
                } else if (obj == rright_btn) {
                    pan_look(PI / 20);
                }
            } else {
                if (obj == up_btn) {
                    spot_move_y(spot_state, -0.005);
                } else if (obj == down_btn) {
                    spot_move_y(spot_state, 0.005);
                } else if (obj == left_btn) {
                    spot_move_x(spot_state, -0.005);
                } else if (obj == right_btn) {
                    spot_move_x(spot_state, 0.005);
                }
            }

            break;

        case LV_EVENT_CLICKED:
            if (in_check_list(ck_group, obj, sizeof(ck_group) / sizeof(lv_obj_t*))) {
                deal_check(ck_group, obj, sizeof(ck_group) / sizeof(lv_obj_t*));
            }

            if (obj == save_btn) {
                save_lookat_param();
                save_spot_param();
            }
    }
}

#define MAX_LBL_LEN 100
void top_view_calib_event_cb(lv_obj_t* obj, lv_event_t event){
    char buf[MAX_LBL_LEN];
    switch(event) {
        case LV_EVENT_VALUE_CHANGED:
            if (obj == slider_box_width) {
                snprintf(buf, MAX_LBL_LEN, "%dcm", lv_slider_get_value(obj));
                lv_label_set_text(lbl_bwidth, buf); 
            } else if (obj == slider_box_height) {
                snprintf(buf, MAX_LBL_LEN, "%dcm", lv_slider_get_value(obj));
                lv_label_set_text(lbl_bheight, buf); 
            } else if (obj == slider_margin) {
                snprintf(buf, MAX_LBL_LEN, "%dcm", lv_slider_get_value(obj));
                lv_label_set_text(lbl_bmargin, buf);
            }
            set_top_param(lv_slider_get_value(slider_box_width), lv_slider_get_value(slider_box_height), lv_slider_get_value(slider_margin));
            update_top_view_matrix();

        case LV_EVENT_CLICKED:
            if (obj == save_btn) {
                save_top_param();
            }
    }
}