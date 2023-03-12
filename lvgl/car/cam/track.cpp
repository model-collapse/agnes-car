#include "filter.h"
#include "model.h"
#include "overlap.h"
#include <opencv2/opencv.hpp>
#include <vector>

void crop_to_top_view(const cv::Mat& src, cv::Mat& dst) {
    int32_t offx = (src.cols - BACK_TOP_WIDTH) / 2;
    dst = src(cv::Rect(offx, 0, BACK_TOP_WIDTH, BACK_TOP_HEIGHT));
}

void poseEstimate(const cv::Mat& src, const cv::Mat& base, cv::Mat& rig_trans) {
    std::vector<cv::Point2f> p0, p1;
    cv::Mat srcGray;
    cv::cvtColor(src, srcGray, cv::COLOR_BGR2GRAY);
    cv::Mat baseGray;
    cv::cvtColor(base, baseGray, cv::COLOR_BGR2GRAY);

    cv::goodFeaturesToTrack(srcGray, p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
    try {
        cv::calcOpticalFlowPyrLK(srcGray, baseGray, p0, p1, status, err, cv::Size(15,15), 2, criteria);
    } catch(cv::Exception e) {
        fprintf(stderr, "Exception thrown: %s\n", e.what());
    }

    if (p0.size() == 0) {
        fprintf(stderr, "p0 size is 0\n");
    } else {
        fprintf(stderr, "p0 size is %d\n", p0.size());
    }


    rig_trans = cv::estimateRigidTransform(p1, p0, false);
}

extern float margin;
extern const int32_t car_width;
cv::Mat base;
void top_view_optf(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache) {
    if (base.empty()) {
        base = src.clone();
        crop_to_top_view(src, dst);
        return;
    }

    cv::Mat trans;
    poseEstimate(src, base, trans);
    if (!(trans.cols == 3 && trans.rows == 2)) {
        fprintf(stderr, "invalid size of tran matrix, [%d,%d]\n", trans.cols, trans.rows);
        trans.create(2, 3, CV_32F);
        trans.at<float>(0, 0) = 1;
        trans.at<float>(0, 1) = 0;
        trans.at<float>(1, 1) = 1;
        trans.at<float>(1, 0) = 0;
        trans.at<float>(0, 2) = 0;
        trans.at<float>(1, 2) = 0;
    }

    cv::Mat tmp;
    cv::warpAffine(base, tmp, trans, src.size());
    float margin_start = (BACK_TOP_HEIGHT - CAR_ICON_HEIGHT) / 5 + CAR_ICON_HEIGHT;
    float margin_size = (float)margin / car_width * CAR_ICON_WIDTH;
    for (int y = margin_start + margin_size; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            tmp.at<cv::Vec3b>(y, x) = src.at<cv::Vec3b>(y, x);
        }
    }
    
    base = tmp;
    crop_to_top_view(tmp, dst);
}


void optf_visual(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache) {
    if (base.empty()) {
        base = src.clone();
        dst = src;
        return;
    }

    float margin_start = (BACK_TOP_HEIGHT - CAR_ICON_HEIGHT) / 5 + CAR_ICON_HEIGHT;
    float margin_size = (float)margin / car_width * CAR_ICON_WIDTH;
    int32_t starty = (int)(margin_start + margin_size);

    std::vector<cv::Point2f> p0, p1;
    cv::Mat srcGray;
    cv::cvtColor(src, srcGray, cv::COLOR_BGR2GRAY);
    cv::Mat baseGray;
    cv::cvtColor(base, baseGray, cv::COLOR_BGR2GRAY);

    cv::goodFeaturesToTrack(srcGray(cv::Rect(0, starty, BACK_TOP_WIDTH*2, BACK_TOP_HEIGHT - starty)), p0, 100, 0.1, 7, cv::Mat(), 7, false, 0.04);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.003);
    try {
        cv::calcOpticalFlowPyrLK(srcGray(cv::Rect(0, starty, BACK_TOP_WIDTH*2, BACK_TOP_HEIGHT - starty)), baseGray(cv::Rect(0, starty, BACK_TOP_WIDTH*2, BACK_TOP_HEIGHT - starty)), p0, p1, status, err, cv::Size(5,5), 2, criteria);
    } catch(cv::Exception e) {
        fprintf(stderr, "Exception thrown: %s\n", e.what());
    }

    /*if (p0.size() == 0) {
        fprintf(stderr, "p0 size is 0\n");
    } else {
        fprintf(stderr, "p0 size is %d\n", p0.size());
    }*/

    base = src.clone();
    dst = src.clone();
    for (int i = 0; i < p0.size(); i++) {
        if (status[i] < 0) continue;

        p0[i].y += starty;
        cv::circle(dst, p0[i], 5, cv::Vec3b(155, 55, 255), -1);
        p1[i].y += starty;
        cv::line(dst, p0[i], p1[i], cv::Vec3b(155, 55, 255), 1);
    }
}