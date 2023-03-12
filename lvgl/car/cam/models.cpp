#include "cam.h"
#include "models.h"
#include "model.hpp"
#include "model.h"
#include "opencv2/opencv.hpp"

#include "overlap.hpp"
#include "filter.hpp"
#include "filter.h"

ViewModel models[MAX_MODELS];

void clear_tagged_model_cache(ViewModel* m) {
    for (int i = 0; i < MAX_FILTERS; i++) {
        if (m->cacheTags[i]) {
            m->filterCache[i].release();
            m->cacheTags[i] = false;
        }
    }
}

void enable_model(int32_t id) {
    models[id].enabled = true;
    for (int i = 0; i < MAX_FILTERS; i++) {
        models[id].cacheTags[i] = true;
    }
}

void disable_model(int32_t id) {
    models[id].enabled = false;
    for (int i = 0; i < MAX_FILTERS; i++) {
        models[id].cacheTags[i] = true;
    }
}

void reset_all_models() {
    for (int i = 0; i < MAX_MODELS; i++) {
        if (models[i].id < 0) {
            break;
        }
        disable_model(i);
    }
}

const lv_img_dsc_t* image_dsc(int32_t model) {
    return &models[model].desc;
}

void init_back_dir() {
    set_lookat_model(MODEL_BACK_DIR);
}

void init_lookdown_calib() {
    set_lookat_model(MODEL_BACK_DOWN);
}

void init_top_calib() {
    set_lookat_model(MODEL_BACK_DOWN);
    set_top_expand_mode(TOP_EXP_MODE_VIEW);
}

void init_track() {
    set_lookat_model(MODEL_BACK_DOWN);
    set_top_expand_mode(TOP_EXP_MODE_TRACK);
}

void init_optf() {
    set_lookat_model(MODEL_BACK_DOWN);
    set_top_expand_mode(TOP_EXP_MODE_TRACK);
}

void init_models() {
    ViewModel& back_eye = models[MODEL_BACK_EYE];
    back_eye.cam = BACK_CAM;
    back_eye.height = BACK_EYE_HEIGHT;
    back_eye.width = BACK_EYE_WIDTH;
    back_eye.filters[0] = regulate_img;
    back_eye.filters[1] = NULL;
    back_eye.overlaps[0] = render_circle;
    back_eye.overlaps[1] = NULL;
    back_eye.id = MODEL_BACK_EYE;
    back_eye.init = NULL;

    ViewModel& back_top = models[MODEL_BACK_TOP];
    back_top.cam = BACK_CAM;
    back_top.height = BACK_TOP_HEIGHT;
    back_top.width = BACK_TOP_WIDTH;
    back_top.filters[0] = regulate_img;
    back_top.filters[1] = extract_top_view;
    back_top.filters[2] = NULL;
    back_top.overlaps[0] = render_car_icon;
    back_top.overlaps[1] = NULL;
    back_top.id = MODEL_BACK_TOP;
    back_top.init = init_top_calib;

    ViewModel& back_dir = models[MODEL_BACK_DIR];
    back_dir.cam = BACK_CAM;
    back_dir.height = BACK_DIR_HEIGHT;
    back_dir.width = BACK_DIR_HEIGHT;
    back_dir.filters[0] = regulate_img;
    back_dir.filters[1] = extract_dir_view;
    back_dir.filters[2] = NULL;
    back_dir.overlaps[0] = NULL;
    back_dir.id = MODEL_BACK_DIR;
    back_dir.init = init_back_dir;

    ViewModel& back_ldw = models[MODEL_BACK_DOWN];
    back_ldw.cam = BACK_CAM;
    back_ldw.height = BACK_DOWN_WIDTH;
    back_ldw.width = BACK_DOWN_HEIGHT;
    back_ldw.filters[0] = regulate_img;
    back_ldw.filters[1] = extract_dir_view;
    back_ldw.filters[2] = NULL;
    back_ldw.filters[3] = NULL;
    back_ldw.overlaps[0] = render_calib_spots;
    back_ldw.overlaps[1] = NULL;
    back_ldw.id = MODEL_BACK_DOWN;
    back_ldw.init = init_lookdown_calib;

    ViewModel& back_track = models[MODEL_BACK_TRACK];
    back_track.cam = BACK_CAM;
    back_track.height = BACK_TOP_HEIGHT;
    back_track.width = BACK_TOP_WIDTH;
    back_track.filters[0] = regulate_img;
    back_track.filters[1] = extract_top_view;
    back_track.filters[2] = top_view_optf;
    back_track.filters[3] = NULL;
    back_track.overlaps[0] = render_car_icon;
    back_track.overlaps[1] = NULL;
    back_track.id = MODEL_BACK_TRACK;
    back_track.init = init_track;

    ViewModel& back_optf = models[MODEL_BACK_OPTF];
    back_optf.cam = BACK_CAM;
    back_optf.height = BACK_TOP_HEIGHT;
    back_optf.width = BACK_TOP_WIDTH * 2;
    back_optf.filters[0] = regulate_img;
    back_optf.filters[1] = extract_top_view;
    back_optf.filters[2] = optf_visual;
    back_optf.filters[3] = NULL;
    back_optf.overlaps[0] = NULL;//render_car_icon;
    back_optf.overlaps[1] = NULL;
    back_optf.id = MODEL_BACK_OPTF;
    back_optf.init = init_optf;

    ViewModel& last = models[MODEL_BACK_OPTF + 1];
    last.id = -1;

    for (int32_t i = 0; i < MAX_MODELS; i++) {
        if (models[i].id < 0) {
            break;
        }
        
        init_model(models + i);
    }
}
