/**
 * @file ImageProfile.h (assuming this is a header file)
 * @brief Header file for the ImageProfile class.
 *
 * This header file defines the ImageProfile class, which is used to analyze and store image statistics.
 */

#ifndef IMAGE_PROFILE_H
#define IMAGE_PROFILE_H

#include "iniparser.h" // Assuming declarations for IniReader, Saver, distributionBox
#include "imghelpers.h"
#include "saver.h"
#include "generic.h"
#include <kll_sketch.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>


// Typedef for distribution box data structure (assuming datasketches::kll_sketch<float>)
typedef datasketches::kll_sketch<float> distributionBox;

/**
 * @class ImageProfile
 * @brief A class for analyzing and storing image statistics.
 *
 * This class provides functionalities for analyzing image properties like distribution of pixel values, contrast, brightness, etc. It utilizes KLL sketches for memory-efficient storage of these statistics.
 */

class ImageProfile {
public:
    /**
     * @brief Destructor to clean up resources
     */
    ~ImageProfile();
    
/**
   * @brief Constructs an ImageProfile object with the specified image characteristics.
   *
   * @param channels The number of channels in the image (e.g., grayscale: 1, RGB: 3).
   * @param img_type The image type (implementation specific).
   * @param metrics The set of image metrics to be tracked (e.g., "contrast", "brightness").
   */

    /**
     * @brief Constructor to initialize ImageProfile object
     * @param conf_path Path to configuration file
     * @param save_interval Interval for saving statistics
     * @param channels Number of image channels (default: 1)
     */
    ImageProfile(const std::string& conf_path, int save_interval, int channels = 1);

   /**
   * @brief Logs image statistics for the provided image data.
   *
   * This function analyzes the provided image data and updates the internal KLL sketches with relevant statistics.
   *
   * @param img The image data as a vector of bytes.
   * @param image_width The width of the image in pixels.
   * @param image_height The height of the image in pixels.
   * @param imgpixels_stat A map to store per-channel statistics (implementation specific).
   * @param imgprofile_map A map to store various image profile metrics (e.g., "contrast").
   */

    /**
     * @brief Computes and logs selected image statistics
     * @param img OpenCV image matrix
     * @param save_sample Flag indicating whether to save samples exceeding thresholds
     * @return 1 on success, error code on failure
     */
    int profile(cv::Mat& img, bool save_sample = false);

#ifndef TEST
private:
#endif
    Saver* saver;
    std::string statSavepath;
    std::string dataSavepath;
    std::map<std::string, std::vector<std::string>> imageConfig;
    int channels;
   /**
   * @brief KLL sketch for storing contrast distribution.
   */
  distributionBox contrastBox;

  /**
   * @brief KLL sketch for storing brightness distribution.
   */
  distributionBox brightnessBox;
  distributionBox sharpnessBox;

  std::vector<distributionBox *> pixelBox;
  /**
   * @brief KLL sketch for storing mean pixel value distribution.
   */
  std::vector<distributionBox *> meanBox;

  /**
   * @brief KLL sketch for storing noise distribution.
   */
  distributionBox noiseBox;


    /**
     * @brief Registers statistics for saving based on configuration
     * @param name Statistic name
     */
    void registerStatistics(const std::string& name);

    /**
     * @brief Computes the specified statistic for an image
     * @param name Statistic name
     * @param img OpenCV image matrix
     * @return Computed statistic value
     */
    float computeStatistic(const std::string& name, cv::Mat& img);

    /**
     * @brief Checks if the statistic value exceeds the configured threshold
     * @param name Statistic name
     * @param stat_score Computed statistic value
     * @param config Threshold configuration
     * @return True if threshold exceeded, otherwise false
     */
    bool isThresholdExceeded(const std::string& name, float stat_score, const std::vector<std::string>& config);

    /**
     * @brief Iterates over an image and applies a callback for each pixel's values
     * @param img OpenCV image matrix
     * @param callback Function to call with each pixel's values
     */
    void iterateImage(const cv::Mat& img, const std::function<void(const std::vector<int>&)>& callback);

    /**
     * @brief Updates the pixel values for histogram calculation
     * @param pixelValues Vector of pixel values
     */
    void updatePixelValues(const std::vector<int>& pixelValues);
};

#endif // IMAGEPROFILE_H
