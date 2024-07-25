/**
 * @file ImageSampler.cpp
 * @brief Implements ImageSampler class for image sampling based on uncertainty
 */
#include "imagesampler.h"
#include "imghelpers.h"
#include "iniparser.h"

ImageSampler::~ImageSampler() {
    delete saver;
}

/**
 * @class ImageSampler
 * @brief Class for selecting uncertain image samples for further analysis
 */
  /**
   * @brief Constructor to initialize ImageSampler object
   * @param conf_path Path to configuration file
   * @param saver Saver object for saving sampling statistics
   */
ImageSampler::ImageSampler(const std::string& conf_path, int save_interval)
    : saver(new Saver(save_interval, "ImageSampler")) {
    try {
        // Read configuration settings
        IniParser parser;
        samplingConfig = parser.parseIniFile(conf_path, "sampling", "");
        statSavepath = samplingConfig["filepath"][0];
        dataSavepath = samplingConfig["filepath"][1];
        createFolderIfNotExists(statSavepath, dataSavepath);
        samplingConfig.erase("filepath");

        // Register sampling statistics for saving based on configuration
        for (const auto& sampleMetric : samplingConfig) {
            registerStatistics(sampleMetric.first);
        }

        saver->StartSaving();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

  /**
   * @brief Selects uncertain image samples based on configured criteria
   * @param results Vector of confidence scores for each image prediction
   * @param uncertainty_sampling Vector indicating uncertainty criteria for each sample
   * @param img OpenCV image matrix
   * @param save_sample Flag indicating whether to save sampled images
   * @return 1 on success, error code on failure
   */

int ImageSampler::sample(const std::vector<std::pair<float, int>>& results, cv::Mat& img, bool save_sample) {
    std::vector<float> confidence; // Extract confidence scores
    for (const auto& pair : results) {
        confidence.push_back(pair.first);
    }

    // Apply configured sampling criteria to identify uncertain samples
    for (const auto& sampleMetric : samplingConfig) {
        try {
            float thresh_lower = std::stof(sampleMetric.second[0]);
            float thresh_upper = std::stof(sampleMetric.second[1]);
	    std::string metric = sampleMetric.first;
            float confidence_score = computeConfidence(metric, confidence);

            updateSamplingStatistics(metric, confidence_score);

            if (confidence_score < thresh_lower || confidence_score > thresh_upper) {
                saveImageWithTimestamp(img, dataSavepath, metric);
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid argument - " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error: Out of range - " << e.what() << std::endl;
        }
    }

    if (!save_sample) {
        saver->StopSaving();
    }

    return 1; // Indicate success
}


  /**
   * @brief Calculates margin confidence (difference between top two probabilities)
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */

   float ImageSampler::margin_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    if (!sorted) {
        std::sort(prob_dist.begin(), prob_dist.end(), std::greater<float>()); // Sort probabilities from largest to smallest
    }
    float difference = prob_dist[0] - prob_dist[1]; // Difference between the top two probabilities
    float margin_conf = 1.0f - difference;
    return margin_conf;
   }

   
  /**
   * @brief Calculates least confidence 
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return least confidence score
   */

    float ImageSampler::least_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    float simple_least_conf;
    if (sorted) {
        simple_least_conf = prob_dist[0]; // Most confident prediction
    } else {
        simple_least_conf = *std::max_element(prob_dist.begin(), prob_dist.end()); // Most confident prediction
    }
    size_t num_labels = prob_dist.size(); // Number of labels
    float normalized_least_conf = (1.0f - simple_least_conf) * (static_cast<float>(num_labels) / (num_labels - 1));
    return normalized_least_conf;
    }


  /**
   * @brief Calculates ratio confidence (difference between top two probabilities)
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Margin confidence score
   */

    float ImageSampler::ratio_confidence(std::vector<float>& prob_dist, bool sorted = false) {
    if (!sorted) {
        std::sort(prob_dist.begin(), prob_dist.end(), std::greater<float>()); // Sort probabilities from largest to smallest
    }
    float ratio_conf = prob_dist[1] / prob_dist[0]; // Ratio between the top two probabilities
    return ratio_conf;
    }


  /**
   * @brief Calculates entropy based confidence
   * @param prob_dist Vector of class probabilities
   * @param sorted Flag indicating if probabilities are already sorted (default: false)
   * @return Entropy confidence score
   */

   float ImageSampler::entropy_confidence(std::vector<float>& prob_dist) {
    float raw_entropy = 0.0;
    size_t num_labels = prob_dist.size();

    for (size_t i = 0; i < num_labels; i++) {
        if (prob_dist[i] > 0.0) {
            raw_entropy -= prob_dist[i] * std::log2(prob_dist[i]); // Multiply each probability by its base 2 log and sum
        }
    }
    float normalized_entropy = raw_entropy / std::log2(static_cast<float>(num_labels));
    return normalized_entropy;
    }

/**
 * @brief Registers sampling statistics for saving based on configuration
 * @param name Sampling confidence name
 */
void ImageSampler::registerStatistics(const std::string& name) {
    if (name == "MARGINCONFIDENCE") {
        saver->AddObjectToSave((void*)(&marginConfidenceBox), KLL_TYPE, statSavepath + "marginconfidence.bin");
    } else if (name == "LEASTCONFIDENCE") {
        saver->AddObjectToSave((void*)(&leastConfidenceBox), KLL_TYPE, statSavepath + "leastconfidence.bin");
    } else if (name == "RATIOCONFIDENCE") {
        saver->AddObjectToSave((void*)(&ratioConfidenceBox), KLL_TYPE, statSavepath + "ratioconfidence.bin");
    } else if (name == "ENTROPYCONFIDENCE") {
        saver->AddObjectToSave((void*)(&entropyConfidenceBox), KLL_TYPE, statSavepath + "entropyconfidence.bin");
    }
}


/**
 * @brief Computes confidence score based on the sampling method
 * @param name Sampling confidence name
 * @param confidence Vector of class probabilities
 * @return Computed confidence score
 */
float ImageSampler::computeConfidence(const std::string& name, std::vector<float>& confidence) {
    if (name == "MARGINCONFIDENCE") {
        return margin_confidence(confidence, false);
    } else if (name == "LEASTCONFIDENCE") {
        return least_confidence(confidence, false);
    } else if (name == "RATIOCONFIDENCE") {
        return ratio_confidence(confidence, false);
    } else if (name == "ENTROPYCONFIDENCE") {
        return entropy_confidence(confidence);
    }
    return -1.0f;
}


/**
 * @brief Updates sampling statistics based on the confidence score
 * @param name Sampling confidence name
 * @param confidence_score Computed confidence score
 */
void ImageSampler::updateSamplingStatistics(const std::string& name, float confidence_score) {
    if (name == "MARGINCONFIDENCE") {
        marginConfidenceBox.update(confidence_score);
    } else if (name == "LEASTCONFIDENCE") {
        leastConfidenceBox.update(confidence_score);
    } else if (name == "RATIOCONFIDENCE") {
        ratioConfidenceBox.update(confidence_score);
    } else if (name == "ENTROPYCONFIDENCE") {
        entropyConfidenceBox.update(confidence_score);
    }
}
