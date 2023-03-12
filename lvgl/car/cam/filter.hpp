#ifndef FILTER_HPP
#define FILTER_HPP

#include <opencv2/opencv.hpp>

void regulate_img(const cv::Mat& rsrc, cv::Mat& dst, cv::Mat& cache);

void extract_lookat_view(const cv::Mat& src, cv::Mat& dst, cv::Point2f center, int32_t radius, cv::Point2f lookAt, double fov, int32_t dsize);
void extract_top_view(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache);

void extract_dir_view(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache);
void extract_lookdown_view(const cv::Mat& src, cv::Mat& dst);

void top_view_optf(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache);
void optf_visual(const cv::Mat& src, cv::Mat& dst, cv::Mat& cache);

#endif