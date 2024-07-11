#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <ctime>


/**
 * @brief Convert an image to grayscale.
 * 
 * @param img The input image.
 * @param grayscale The output grayscale image.
 * @return int 0 on success.
 */
int convertGrayScale(cv::Mat &img, cv::Mat &grayscale) {
    cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    return 0;
}

/**
 * @brief Calculate the sharpness of an image using the Sobel operator.
 * 
 * @param image The input image.
 * @return double The calculated sharpness.
 */
double calculateSharpnessSobel(cv::Mat &image) {
    cv::Mat grayscale;
    if (image.channels() == 3 && image.depth() == CV_8U) {
        cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);
    } else {
        grayscale = image.clone();
    }

    cv::Mat sobelX, sobelY;
    cv::Sobel(grayscale, sobelX, CV_64F, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(grayscale, sobelY, CV_64F, 0, 1, cv::BORDER_REPLICATE);

    cv::Mat sobelXSquared, sobelYSquared;
    cv::multiply(sobelX, sobelX, sobelXSquared);
    cv::multiply(sobelY, sobelY, sobelYSquared);

    cv::Scalar meanSobelXSquared = cv::mean(sobelXSquared);
    cv::Scalar meanSobelYSquared = cv::mean(sobelYSquared);

    double sharpness = std::sqrt(meanSobelXSquared[0] + meanSobelYSquared[0]);

    return sharpness;
}

/**
 * @brief Calculate the sharpness of an image using the Laplacian operator.
 * 
 * @param img The input image.
 * @return double The calculated sharpness.
 */
double calculateSharpnessLaplacian(cv::Mat &img) {
    cv::Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        grayscale = img;
    }

    cv::Mat laplacian;
    cv::Laplacian(grayscale, laplacian, CV_64F);

    cv::Scalar mean, sigma;
    cv::meanStdDev(laplacian, mean, sigma);

    double sharpness = sigma.val[0];

    return sharpness;
}

/**
 * @brief Calculate the Signal-to-Noise Ratio (SNR) of an image.
 * 
 * @param img The input image.
 * @return double The calculated SNR.
 */
double calculateSNR(cv::Mat &img) {
    cv::Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        grayscale = img;
    }

    cv::Scalar mean, sigma;
    cv::meanStdDev(grayscale, mean, sigma);

    double signal = mean[0];
    double noise = sigma[0];

    if (noise == 0) {
        return std::numeric_limits<double>::infinity();
    }

    double snr = 20 * std::log10(signal / noise + 1e-7);

    return snr;
}

/**
 * @brief Calculate the mean of each channel in an image.
 * 
 * @param image The input image.
 * @return std::vector<double> A vector containing the mean of each channel.
 */
std::vector<double> calculateChannelMeans(const cv::Mat &image) {
    std::vector<double> channelMeans;
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    for (size_t i = 0; i < channels.size(); ++i) {
        cv::Scalar meanValue = cv::mean(channels[i]);
        channelMeans.push_back(meanValue[0]);
    }

    return channelMeans;
}

/**
 * @brief Calculate the contrast of an image.
 * 
 * @param img The input image.
 * @return double The calculated contrast.
 */
double calculateContrast(cv::Mat &img) {
    cv::Mat grayscale;
    if (img.channels() == 3) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, grayscale, cv::COLOR_BGRA2GRAY);
    } else {
        grayscale = img;
    }

    double minPixelValue, maxPixelValue;
    cv::minMaxLoc(grayscale, &minPixelValue, &maxPixelValue);

    return (maxPixelValue - minPixelValue) / maxPixelValue;
}

/**
 * @brief Calculate the brightness of an image.
 * 
 * @param img The input image.
 * @return double The calculated brightness.
 */
double calculateBrightness(cv::Mat &img) {
    std::vector<double> meanValues;
    if (img.channels() == 3) {
        meanValues = calculateChannelMeans(img);
        return meanValues[0] * 0.299 + meanValues[1] * 0.587 + meanValues[2] * 0.114;
    } else {
        cv::Scalar meanValue = cv::mean(img);
        return meanValue[0];
    }
}

/**
 * @brief Save an image with an incremental name in the specified directory.
 * 
 * @param img The input image.
 * @param path The directory path.
 * @param baseName The base name for the image files.
 * @return std::string The full path of the saved image.
 */
std::string saveImageWithIncrementalName(const cv::Mat &img, const std::string &path, const std::string &baseName) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            perror("Error creating directory");
            return "";
        }
    }

    int highestIndex = 0;
    DIR *dir = opendir(path.c_str());
    if (dir) {
        dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                std::string filename(entry->d_name);
                if (filename.find(baseName) == 0) {
                    int index = std::stoi(filename.substr(baseName.length()));
                    highestIndex = std::max(highestIndex, index);
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        return "";
    }

    int newIndex = highestIndex + 1;

    std::stringstream ss;
    ss << path << "/" << baseName << std::setfill('0') << std::setw(4) << newIndex << ".png";
    std::string newFilename = ss.str();

    cv::imwrite(newFilename, img);

    return newFilename;
}

/**
 * @brief Save an image with a timestamped name in the specified directory.
 * 
 * @param img The input image.
 * @param path The directory path.
 * @param baseName The base name for the image files.
 * @return std::string The full path of the saved image.
 */
std::string saveImageWithTimestamp(const cv::Mat &img, const std::string &path, const std::string &baseName) {
    // Check if the directory exists, if not, create it
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            perror("Error creating directory");
            return "";
        }
    }

    // Get current time with microseconds granularity
    auto now = std::chrono::system_clock::now();
    auto now_since_epoch = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now_since_epoch);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now_since_epoch) % 1000000;

    // Combine seconds and microseconds to get the full timestamp
    std::stringstream ss;
    ss << path << "/" << baseName << "_" << seconds.count() << std::setw(6) << std::setfill('0') << microseconds.count() << ".png";
    std::string newFilename = ss.str();

    // Save the image
    cv::imwrite(newFilename, img);

    return newFilename;
}

