#ifndef OVERLAP_HPP
#define OVERLAP_HPP

#include <opencv2/opencv.hpp>

void render_aim_circle(cv::Mat& frame, cv::Point center, double radius);
void render_circle(cv::Mat& frame);
void render_3x3_net(cv::Mat& frame);
void render_calib_spots(cv::Mat& frame);
void render_car_icon(cv::Mat& frame);

#endif