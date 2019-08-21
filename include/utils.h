#pragma once

#include <string>
#include <opencv2/imgproc.hpp>

std::string GetMatDepth(const cv::Mat& mat);
std::string GetMatType(const cv::Mat& mat);
std::string GetBaseFilename(const std::string &filename);