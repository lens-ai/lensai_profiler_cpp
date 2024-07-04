/**
 * @file ConfidenceMetrics.h
 * @brief Header file for ImageSampler class and confidence metric calculations
 */

#ifndef CONFIDENCE_METRICS_H
#define CONFIDENCE_METRICS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <kll_sketch.hpp>

#include "iniparser.h"
#include "saver.h"
#include "objectuploader.h"
#include "generic.h"

// Typedef for distribution box data structure (assuming datasketches::kll_sketch<unit>)
typedef datasketches::kll_sketch<float> distributionBox;

/**
 * @class ImageSampler
 * @brief Class for selecting uncertain image samples for further analysis based on various confidence metrics
 */
class ImageSampler {
public:
  /**
   * @brief Constructor to initialize ImageSampler object with configuration file path
   * @param configFilePath Path to the configuration file
   */
  explicit ImageSampler(std::string conf_path, int save_interval);
  ~ImageSampler();

  /**
   * @brief Selects uncertain image samples based on configured criteria
   * @param classificationResults Vector of confidence scores for each image prediction
   * @param image OpenCV image matrix
   * @param saveSample Flag indicating whether to save sampled images
   */
   int sample(std::vector<std::pair<float, int>> &results, cv::Mat &img, bool save_sample);
  
   /**
   * @brief Calculates margin confidence (difference between top two probabilities)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */
   float margin_confidence(std::vector<float>& prob_dist, bool sorted);

  /**
   * @brief Calculates least confidence (normalized maximum probability)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Least confidence score
   */
    float least_confidence(std::vector<float>& prob_dist, bool sorted);

  /**
   * @brief Calculates ratio confidence (ratio of top two probabilities)
   * @param probabilityDistribution Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Ratio confidence score
   */
    float ratio_confidence(std::vector<float>& prob_dist, bool sorted);

  /**
   * @brief Calculates entropy-based confidence
   * @param probabilityDistribution Vector of class probabilities
   * @return Entropy-based confidence score
   */
   float entropy_confidence(std::vector<float>& prob_dist);
   std::string filesSavePath;
private:   
  // Member variables for storing confidence metric statistics
  distributionBox marginConfidenceBox;
  distributionBox leastConfidenceBox;
  distributionBox ratioConfidenceBox;
  distributionBox entropyConfidenceBox;
    Saver *saver;
    //ImageUploader *uploader;
	std::map<std::string, std::vector<std::string>> samplingConfig;
};

#endif // CONFIDENCE_METRICS_H

