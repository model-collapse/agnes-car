#include "filter.h"
#include "filter.hpp"
#include "overlap.h"
#include "overlap.hpp"
#include "math.h"
#include "model.h"
#include <stdio.h>
#include <iostream>

struct LookAtAndFov3D {
    cv::Vec3f lookAt;
    cv::Vec3f u;
    cv::Vec3f v;

    double fov;
};

LookAtAndFov3D look_dir3;
LookAtAndFov3D look_down3;
LookAtAndFov3D* look_cur;

static void extract_lookat_view_l3d(const cv::Mat& rsrc, cv::Mat& dst, cv::Point2f center, double radius, LookAtAndFov3D* lkf, int32_t dsize);

double l2LenP2f(cv::Vec2f v) {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

cv::Vec2f l2NormV2(cv::Vec2f p, double ln) {
    double r = ln / l2LenP2f(p);
    //return cv:Vec2f(p[0] * r, p[1] * r);
    return p * r;
}

double l2LenP3f(cv::Vec3f v) {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

cv::Vec3f l2NormV3(cv::Vec3f p, double ln) {
    double r = ln / l2LenP3f(p);
    //return cv:Vec2f(p[0] * r, p[1] * r, p[2] * r);
    return p * r;
}

cv::Vec3f scaleV3(cv::Vec3f p, double r) {
    return cv::Vec3f(p[0] * r, p[1] * r, p[2] * r);
}

cv::Vec3f crossV3(cv::Vec3f p1, cv::Vec3f p2) {
    return cv::Vec3f(p1[1]*p2[2] - p1[2]*p2[1], p1[2]*p2[0] - p1[0]*p2[2], p1[0]*p2[1] - p1[1]*p2[0]);
}

#define min(a, b) (a < b) ? a : b
void extract_dir_view(const cv::Mat& src, cv::Mat& dst) {
    int32_t cx;
    int32_t cy;
    int32_t r;

    //fprintf(stderr, "dir\n");
    get_circle_param(&cx, &cy, &r);
    
    float fcx = (float)cx / BACK_EYE_WIDTH;
    float fcy = (float)cy / BACK_EYE_HEIGHT;
    float fr = (float)r / BACK_EYE_WIDTH;

    extract_lookat_view_l3d(src, dst, cv::Point2f(fcx, fcy), fr, look_cur, BACK_DIR_WIDTH);
}

void extract_lookdown_view(const cv::Mat& src, cv::Mat& dst) {
}

#define max(a, b) (a > b) ? a : b
void regulate_img(const cv::Mat& rsrc, cv::Mat& dst) {
    if (rsrc.cols > rsrc.rows) {
        int32_t offx = (rsrc.cols - rsrc.rows) / 2;
        dst.create(cv::Size(rsrc.cols, rsrc.cols), CV_8UC3);
        cv::Mat patch = dst(cv::Rect(0, offx, rsrc.cols, rsrc.rows));
        rsrc.copyTo(patch);
        //dst = rsrc(cv::Rect(offx, 0, rsrc.cols - 2*offx, rsrc.rows));
    } else {
        int32_t offy = (rsrc.rows - rsrc.cols) / 2;
        dst.create(cv::Size(rsrc.rows, rsrc.rows), CV_8UC3);
        //dst = rsrc(cv::Rect(0, offy, rsrc.cols, rsrc.rows - 2*offy));
        cv::Mat patch = dst(cv::Rect(offy, 0, rsrc.cols, rsrc.rows));
        rsrc.copyTo(patch);
    }

    return;
}

static void regulate_uv(LookAtAndFov3D* lk) {
    if (look_cur != NULL) {
        lk->u = l2NormV3(crossV3(lk->lookAt, lk->v), -1.0);
        lk->v = l2NormV3(crossV3(lk->lookAt, lk->u), 1.0);
    }
}

static void print_vec3f(const char* prefix, cv::Vec3f v) {
    fprintf(stderr, "%s = [%f, %f, %f]\n", prefix, v[0], v[1], v[2]);
}

void move_look_vertical(double v) {
    if (look_cur != NULL) {
        look_cur->lookAt = l2NormV3(look_cur->lookAt + look_cur->v * v, 1.0);
        regulate_uv(look_cur);
    }
}

void move_look_horizontal(double v) {
    if (look_cur != NULL) {
        look_cur->lookAt = l2NormV3(look_cur->lookAt + look_cur->u * v, 1.0);
        regulate_uv(look_cur);
    }
}

void pan_look(double v) {
    if (look_cur != NULL) {
        double sinv = sin(v);
        double cosv = cos(v);

        look_cur->u = look_cur->u * cosv - look_cur->v * sinv;
        look_cur->v = look_cur->u * sinv + look_cur->v * cosv;
    }
}

void incr_fov(double v) {
    if (look_cur) {
        look_cur->fov += v;
        //fprintf(stderr, "fov = %f, v = %f\n", look_cur->fov, v);
    }
}

struct transformCtx {
    cv::Point2f center;
    float radius;
    LookAtAndFov3D* lkf;
};

int32_t sph_view_trans(cv::Vec2f s, transformCtx& t, cv::Vec2i& nloc) {
    float x = s[0];
    float y = s[1];
    cv::Vec3f locOnPane = t.lkf->lookAt + t.lkf->u * x + t.lkf->v * y;
    cv::Vec3f locOnDome = l2NormV3(locOnPane, 1.0);

    if (locOnDome[2] < 0) {
        return -1;
    }

    double gamma = acos(locOnDome[2]);
    nloc = l2NormV2(cv::Vec2f(locOnDome[0], locOnDome[1]), (gamma*2.0/CV_PI)*t.radius);
    if (sqrt(nloc[0]*nloc[0]+nloc[1]*nloc[1]) > t.radius) {
        return -1;
    } else {
        int slocX = (int)(nloc[0] + t.center.x);
		int slocY = (int)(nloc[1] + t.center.y);
        nloc = cv::Vec2i(slocX, slocY);
    }
    
    return 0;
}

void extract_lookat_view_l3d(const cv::Mat& rsrc, cv::Mat& dst, cv::Point2f center, double radius, LookAtAndFov3D* lkf, int32_t dsize) {
    if (dsize%2 != 1) {
        dsize += 1;
    }

    cv::Mat src = rsrc;
    if (rsrc.cols > rsrc.rows) {
        int32_t offx = (rsrc.cols - rsrc.rows) / 2;
        src = rsrc(cv::Rect(offx, 0, rsrc.cols - 2*offx, rsrc.rows));
    } else {
        int32_t offy = (rsrc.rows - rsrc.cols) / 2;
        src = rsrc(cv::Rect(0, offy, rsrc.cols, rsrc.rows - 2*offy));
    }

    int32_t scl = min(src.cols, src.rows);
    transformCtx ctx{cv::Point2f{center.x * scl, center.y * scl}, radius * scl, lkf};

    int32_t hdsize = dsize / 2;
    dst.create(cv::Size(dsize, dsize), CV_8UC3);

    double projectUnit = tan((lkf->fov/180.0)*(CV_PI/2.0)) / hdsize;
    for (int32_t y = -hdsize; y < hdsize; y++) {
		for (int32_t x = -hdsize; x < hdsize; x++) {
            cv::Vec2i sc;
            cv::Vec3b val;
            int32_t rtc = sph_view_trans(cv::Vec2f(x * projectUnit, y * projectUnit), ctx, sc);
            if (rtc) {
                val = cv::Vec3b(0, 0, 0);
            } else {
                int slocX = sc[0];
                int slocY = sc[1];
                if (slocX < 0 || slocX >= src.cols || slocY < 0 || slocY >= src.rows) {
				    val = cv::Vec3b(0, 255, 0);
			    } else {
                    val = src.at<cv::Vec3b>(sc[1], sc[0]);
                }
            }
            
            dst.at<cv::Vec3b>(y+hdsize, x+hdsize) = val;
        }
    }
}

void extract_lookat_view_l3ddd(const cv::Mat& rsrc, cv::Mat& dst, cv::Point2f center, double radius, LookAtAndFov3D* lkf, int32_t dsize) {
    if (dsize%2 != 1) {
        dsize += 1;
    }

    cv::Mat src = rsrc;
    if (rsrc.cols > rsrc.rows) {
        int32_t offx = (rsrc.cols - rsrc.rows) / 2;
        src = rsrc(cv::Rect(offx, 0, rsrc.cols - 2*offx, rsrc.rows));
    } else {
        int32_t offy = (rsrc.rows - rsrc.cols) / 2;
        src = rsrc(cv::Rect(0, offy, rsrc.cols, rsrc.rows - 2*offy));
    }

    int32_t scl = min(src.cols, src.rows);
    center.x *= scl;
    center.y *= scl;
    radius *= scl;

    int32_t hdsize = dsize / 2;
    dst.create(cv::Size(dsize, dsize), CV_8UC3);

    double projectUnit = tan((lkf->fov/180.0)*(CV_PI/2.0)) / hdsize;
    cv::Vec3f qn = lkf->u * projectUnit;
    cv::Vec3f qd = lkf->v * projectUnit;
    for (int32_t y = -hdsize; y < hdsize; y++) {
		for (int32_t x = -hdsize; x < hdsize; x++) {
            cv::Vec3f locOnPane = lkf->lookAt + qn * (float)x;
			locOnPane += qd * (float)y;
			cv::Vec3f locOnDome = l2NormV3(locOnPane, 1.0);

			double gamma = acos(locOnDome[2]);
			cv::Vec2f nloc = l2NormV2(cv::Vec2f(locOnDome[0], locOnDome[1]), (gamma*2.0/CV_PI)*radius);
            cv::Vec3b val;

			if (sqrt(nloc[0]*nloc[0]+nloc[1]*nloc[1]) > radius) {
				val = cv::Vec3b(0, 0, 0);
			} else {
                double slocX = (int)(nloc[0] + center.x);
			    double slocY = (int)(nloc[1] + center.y);
                if (slocX < 0 || slocX >= src.cols || slocY < 0 || slocY >= src.rows) {
				    val = cv::Vec3b(0, 255, 0);
			    } else {
                    val = src.at<cv::Vec3b>((int)slocY, (int)slocX);
                }
            }

		    dst.at<cv::Vec3b>(y+hdsize, x+hdsize) = val;
        }
    }

    return;
}

#define MAX_CONF_LINE_LEN 100
void restore_lookat_param() {
    FILE* f = fopen(LOOK_AT_PARAM_PATH, "r");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open look_at configure file for read\n");
        restore_default_lookat_param();
        return;
    }

    char* line = new char[MAX_CONF_LINE_LEN];
    size_t line_len;
    float x, y, z, fov;
    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f, %f", &x, &y, &z, &fov);
    look_dir3.fov = fov;
    look_dir3.lookAt = cv::Vec3f(x, y, z);
    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f", &x, &y, &z);
    look_dir3.u = cv::Vec3f(x, y, z);
    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f", &x, &y, &z);
    look_dir3.v = cv::Vec3f(x, y, z);

    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f, %f", &x, &y, &z, &fov);
    look_down3.fov = fov;
    look_down3.lookAt = cv::Vec3f(x, y, z);
    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f", &x, &y, &z);
    look_down3.u = cv::Vec3f(x, y, z);
    getline(&line, &line_len, f);
    sscanf(line, "%f, %f, %f", &x, &y, &z);
    look_down3.v = cv::Vec3f(x, y, z);
    delete [] line;

    fclose(f);
}

void save_lookat_param() {
    FILE* f = fopen(LOOK_AT_PARAM_PATH, "w");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open look_at configure file for write\n");
        return;
    }

    fprintf(f, "%f, %f, %f, %f\n", look_dir3.lookAt[0], look_dir3.lookAt[1], look_dir3.lookAt[2], look_dir3.fov);
    fprintf(f, "%f, %f, %f\n", look_dir3.u[0], look_dir3.u[1], look_dir3.u[2]);
    fprintf(f, "%f, %f, %f\n", look_dir3.v[0], look_dir3.v[1], look_dir3.v[2]);

    fprintf(f, "%f, %f, %f, %f\n", look_down3.lookAt[0], look_down3.lookAt[1], look_down3.lookAt[2], look_down3.fov);
    fprintf(f, "%f, %f, %f\n", look_down3.u[0], look_down3.u[1], look_down3.u[2]);
    fprintf(f, "%f, %f, %f\n", look_down3.v[0], look_down3.v[1], look_down3.v[2]);

    fclose(f);
}

void restore_default_lookat_param() {
    look_dir3.fov = 90;
    look_dir3.lookAt = cv::Vec3f(0, 0, 1);
    look_dir3.u = cv::Vec3f(1.0, 0, 0);
    look_dir3.v = cv::Vec3f(0, 1.0, 0);

    look_down3.fov = 90;
    look_down3.lookAt = cv::Vec3f(0, 0, 1);
    look_down3.u = cv::Vec3f(1.0, 0, 0);
    look_down3.v = cv::Vec3f(0, 1.0, 0);
}

void set_lookat_model(int32_t model) {
    if (model == MODEL_BACK_DIR) {
        look_cur = &look_dir3;
    } else if (model == MODEL_BACK_DOWN) {
        look_cur = &look_down3;
    }
}

float box_width;
float box_height;
float margin;
void set_top_param(float bw, float bh, float mg) {
    box_width = bw;
    box_height = bh;
    margin = mg;
}

void get_top_param(float* bw, float* bh, float* mg) {
    *bw = box_width;
    *bh = box_height;
    *mg = margin;
}

void save_top_param() {
    FILE* f = fopen(TOP_PARAM_PATH, "w");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open top configure file for write\n");
        return;
    }

    fprintf(f, "%f %f %f", box_width, box_height, margin);

    fclose(f);
}

void restore_default_top_param() {
    box_height = 10;
    box_width = 10;
    margin = 10;
}

void restore_top_param() {
    FILE* f = fopen(TOP_PARAM_PATH, "r");
    if (!f) {
        fprintf(stderr, "[FATAL] cannot open top configure file for read\n");
        restore_default_top_param();
        return;
    }

    fscanf(f, "%f %f %f", &box_width, &box_height, &margin);

    update_top_view_matrix();
    fclose(f);
}

void print_points(const char* name, float pts[][2], int32_t size) {
    fprintf(stderr, "Points of %s:\n", name);
    for (int i = 0; i < size; i++) {
        fprintf(stderr, "- %f, %f\n", pts[i][0], pts[i][1]); 
    }
}

extern const int32_t car_width;
extern const int32_t car_length;
cv::Mat top_view_matrix;
float dst_points_g[4][2];
void update_top_view_matrix() {
    float real_view_width = car_width / CAR_ICON_WIDTH * BACK_TOP_WIDTH;
    float margin_start = (BACK_TOP_HEIGHT - CAR_ICON_HEIGHT) / 5 + CAR_ICON_HEIGHT;
    float margin_size = (float)margin / car_width * CAR_ICON_WIDTH;
    float box_w_size = (float)box_width / car_width * CAR_ICON_WIDTH;
    float box_h_size = (float)box_height / car_width * CAR_ICON_WIDTH;
    float offset_x = (BACK_TOP_WIDTH - box_w_size) / 2;
    float offset_y = margin_start + margin_size;

    float dst_points[4][2] = {
        { offset_x + box_w_size, offset_y + box_h_size },
        { offset_x, offset_y + box_h_size },
        { offset_x + box_w_size, offset_y },
        { offset_x, offset_y }
    };

    for (int i = 0; i < 4; i++) {
        dst_points_g[i][0] = dst_points[i][0];
        dst_points_g[i][1] = dst_points[i][1];
    }

    float src_points[4][2];
    get_spot_locs((float*)src_points[0]);
    for (int i = 0; i < 4; i++) {
        src_points[i][0] *= BACK_DOWN_WIDTH;
        src_points[i][1] *= BACK_DOWN_HEIGHT;
    }

    print_points("src", src_points, 4);
    print_points("dst", dst_points, 4);

    top_view_matrix = getPerspectiveTransform(cv::Mat(4, 2, CV_32F, dst_points), cv::Mat(4, 2, CV_32F, src_points));
    fprintf(stderr, "matrix calc: %dx%d\n", top_view_matrix.rows, top_view_matrix.cols);

    std::cout << top_view_matrix << std::endl;
}

cv::Vec2f warpPoint(const float m[3][3], const cv::Vec2f& v) {
    cv::Vec2f ret;
    ret[0] = (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2]) / (m[2][0] * v[0] + m[2][1] * v[1] + m[2][2]);
    ret[1] = (m[1][0] * v[0] + m[1][1] * v[1] + m[1][2]) / (m[2][0] * v[0] + m[2][1] * v[1] + m[2][2]);

    return ret;
}

int32_t top_exp_mode = TOP_EXP_MODE_VIEW;
void set_top_expand_mode(int32_t mode) {
    top_exp_mode = mode;
}

int32_t get_top_expand_mode() {
    return top_exp_mode;
}

void extract_top_view(const cv::Mat& src, cv::Mat& dst) {
    cv::Mat tmp;

    int32_t cx;
    int32_t cy;
    int32_t r;
    get_circle_param(&cx, &cy, &r);
    float fcx = (float)cx / BACK_EYE_WIDTH;
    float fcy = (float)cy / BACK_EYE_HEIGHT;
    float fr = (float)r / BACK_EYE_WIDTH;

    cv::Mat rsrc;
    if (src.cols > src.rows) {
        int32_t offx = (src.cols - src.rows) / 2;
        rsrc = src(cv::Rect(offx, 0, src.cols - 2*offx, src.rows));
    } else {
        int32_t offy = (src.rows - src.cols) / 2;
        rsrc = src(cv::Rect(0, offy, src.cols, src.rows - 2*offy));
    }

    int32_t scl = min(src.cols, src.rows);
    transformCtx ctx{cv::Point2f{fcx * scl, fcy * scl}, fr * scl, &look_down3};
    int32_t dst_width = BACK_TOP_WIDTH;
    int32_t start_x = 0;
    if (top_exp_mode == TOP_EXP_MODE_TRACK) {
        dst_width *= 2;
        start_x = BACK_TOP_WIDTH / 2;
    }
    dst.create(BACK_TOP_HEIGHT, dst_width, CV_8UC3);
    dst.setTo(cv::Vec3b(1, 1, 1));

    cv::Mat tvmi = top_view_matrix;
    float tvmia[3][3];
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            tvmia[y][x] = (float)tvmi.at<double>(y, x);
        }
    }

    float margin_start = (BACK_TOP_HEIGHT - CAR_ICON_HEIGHT) / 5 + CAR_ICON_HEIGHT;
    float margin_size = (float)margin / car_width * CAR_ICON_WIDTH;

    int32_t hdsize = BACK_DOWN_WIDTH / 2;
    double projectUnit = tan((look_down3.fov/180.0)*(CV_PI/2.0)) / hdsize;
    for (int y = (int)(margin_start + margin_size); y < BACK_TOP_HEIGHT; y++) {
        for (int x = 0; x < dst_width; x++) {
            cv::Vec2f imc = warpPoint(tvmia, cv::Vec2d((float)x - start_x, (float)y));
            imc[0] -= hdsize;
            imc[1] -= hdsize;

            cv::Vec2i sc;
            int32_t rc = sph_view_trans(cv::Vec2f(imc[0] * projectUnit, imc[1] * projectUnit), ctx, sc);
            cv::Vec3b val = cv::Vec3b(10, 10, 10);
            if (!rc) {
                int slocX = sc[0];
                int slocY = sc[1];
                if (slocX < 0 || slocX >= src.cols || slocY < 0 || slocY >= src.rows) {
				    val = cv::Vec3b(0, 255, 0);
			    } else {
                    val = rsrc.at<cv::Vec3b>(sc[1], sc[0]);
                }
            }

            dst.at<cv::Vec3b>(y, x) = val;
        }
    }
}