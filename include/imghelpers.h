#ifndef IMGHELPERS_H
#define IMGHELPERS_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

/**
 * @brief Convert an image to grayscale.
 * 
 * @param img The input image.
 * @param grayscale The output grayscale image.
 * @return int 0 on success.
 */
int convertGrayScale(cv::Mat &img, cv::Mat &grayscale);

/**
 * @brief Calculate the sharpness of an image using the Sobel operator.
 * 
 * @param image The input image.
 * @return double The calculated sharpness.
 */
double calculateSharpnessSobel(cv::Mat &image);

/**
 * @brief Calculate the sharpness of an image using the Laplacian operator.
 * 
 * @param img The input image.
 * @return double The calculated sharpness.
 */
double calculateSharpnessLaplacian(cv::Mat &img);

/**
 * @brief Calculate the Signal-to-Noise Ratio (SNR) of an image.
 * 
 * @param img The input image.
 * @return double The calculated SNR.
 */
double calculateSNR(cv::Mat &img);

/**
 * @brief Calculate the mean of each channel in an image.
 * 
 * @param image The input image.
 * @return std::vector<double> A vector containing the mean of each channel.
 */
std::vector<double> calculateChannelMeans(const cv::Mat &image);

/**
 * @brief Calculate the contrast of an image.
 * 
 * @param img The input image.
 * @return double The calculated contrast.
 */
double calculateContrast(cv::Mat &img);

/**
 * @brief Calculate the brightness of an image.
 * 
 * @param img The input image.
 * @return double The calculated brightness.
 */
double calculateBrightness(cv::Mat &img);

/**
 * @brief Save an image with an incremental name in the specified directory.
 * 
 * @param img The input image.
 * @param path The directory path.
 * @param baseName The base name for the image files.
 * @return std::string The full path of the saved image.
 */
std::string saveImageWithIncrementalName(const cv::Mat &img, const std::string &path, const std::string &baseName);

/**
 * @brief Save an image with a timestamped name in the specified directory.
 * 
 * @param img The input image.
 * @param path The directory path.
 * @param baseName The base name for the image files.
 * @return std::string The full path of the saved image.
 */
std::string saveImageWithTimestamp(const cv::Mat &img, const std::string &path, const std::string &baseName);

#endif // IMGHELPERS_H
