/**
 * @file ImageProfile.cpp
 * @brief Implements ImageProfile class for image analysis
 */

#include <vector>
#include <cmath>
#include "imageprofile.h"
#include <iniparser.h>

ImageProfile::~ImageProfile() {
    delete saver;
    for (const auto& obj :  meanBox)
        delete obj;
    for (const auto& obj : pixelBox)
	delete obj;    
}


/**
 * @class ImageProfile
 * @brief Class for computing and managing various image statistics
 */

/**
 * @brief Constructor to initialize ImageProfile object
 * @param conf_path Path to configuration file
 * @param save_interval Interval for saving statistics
 * @param channels Number of image channels (default: 1)
 */
ImageProfile::ImageProfile(const std::string& conf_path, int save_interval, int channels)
    : saver(new Saver(save_interval, "ImageProfile")), channels(channels) {
    try {
        // Read configuration settings
        IniParser parser;
        imageConfig = parser.parseIniFile(conf_path, "image", "");
	statSavepath = imageConfig["filepath"][0];
	dataSavepath = 	imageConfig["filepath"][1];
	std::cout << statSavepath << dataSavepath << std::endl;
        createFolderIfNotExists(statSavepath, dataSavepath);
        imageConfig.erase("filepath");

        // Register statistics for saving based on configuration
        for (const auto& config : imageConfig) {
            registerStatistics(config.first);
        }

        saver->StartSaving();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * @brief Computes and logs selected image statistics
 * @param img OpenCV image matrix
 * @param save_sample Flag indicating whether to save samples exceeding thresholds
 * @return 1 on success, error code on failure
 */
int ImageProfile::profile(cv::Mat& img, bool save_sample) {
    for (const auto& config : imageConfig) {
        float stat_score = computeStatistic(config.first, img);

        if (save_sample && isThresholdExceeded(config.first, stat_score, config.second)) {
            saveImageWithTimestamp(img, dataSavepath, config.first);
        }
    }
    return 1; // Indicate success
}

/**
 * @brief Registers statistics for saving based on configuration
 * @param name Statistic name
 */
void ImageProfile::registerStatistics(const std::string& name) {
    if (name == "NOISE") {
        saver->AddObjectToSave((void*)(&noiseBox), KLL_TYPE, statSavepath + "noise.bin");
    } else if (name == "BRIGHTNESS") {
        saver->AddObjectToSave((void*)(&brightnessBox), KLL_TYPE, statSavepath + "brightness.bin");
    } else if (name == "SHARPNESS") {
        saver->AddObjectToSave((void*)(&sharpnessBox), KLL_TYPE, statSavepath + "sharpness.bin");
    } else if (name == "MEAN") {
        for (int i = 0; i < channels; ++i) {
            auto* dbox = new distributionBox(200);
            meanBox.push_back(dbox);
            saver->AddObjectToSave((void*)(dbox), KLL_TYPE, statSavepath + "mean_" + std::to_string(i) + ".bin");
        }
    } else if (name == "HISTOGRAM") {
        for (int i = 0; i < channels; ++i) {
            auto* dbox_hist = new distributionBox(200);
            pixelBox.push_back(dbox_hist);
            saver->AddObjectToSave((void*)(dbox_hist), KLL_TYPE, statSavepath + "pixel_" + std::to_string(i) + ".bin");
        }
    }
}

/**
 * @brief Computes the specified statistic for an image
 * @param name Statistic name
 * @param img OpenCV image matrix
 * @return Computed statistic value
 */
float ImageProfile::computeStatistic(const std::string& name, cv::Mat& img) {
    float stat_score;
    if (name == "NOISE") {
	stat_score = calculateSNR(img);  
        noiseBox.update(stat_score);
        return -1.0;
    } else if (name == "BRIGHTNESS") {
        stat_score = calculateBrightness(img);
	brightnessBox.update(stat_score);
        return -1.0;		
    } else if (name == "SHARPNESS") {
	stat_score =calculateSharpnessLaplacian(img);     
        sharpnessBox.update(stat_score);
	return -1.0;
    } else if (name == "MEAN") {
        cv::Scalar mean_values = cv::mean(img);
        for (int i = 0; i < img.channels(); ++i) {
            meanBox[i]->update(mean_values[i]);
        }
        return -1.0f; // MEAN doesn't have a single return value
    } else if (name == "CONTRAST") {
	stat_score = calculateContrast(img);    
        contrastBox.update(stat_score);
	return -1.0;
    } else if (name == "HISTOGRAM") {
        iterateImage(img, [this](const std::vector<int>& pixelValues) {
            this->updatePixelValues(pixelValues);
        });
        return -1.0f; // HISTOGRAM doesn't have a single return value
    }
    return -1.0f; // Default case
}


/**
 * @brief Checks if the statistic value exceeds the configured threshold
 * @param name Statistic name
 * @param stat_score Computed statistic value
 * @param config Threshold configuration
 * @return True if threshold exceeded, otherwise false
 */
bool ImageProfile::isThresholdExceeded(const std::string& name, float stat_score, const std::vector<std::string>& config) {
    if (name == "NOISE" || name == "SHARPNESS" || name == "CONTRAST"|| name == "BRIGHTNESS"  )  {
        float threshold_lower = std::stof(config[0]);
        float threshold_upper = std::stof(config[1]);
	return stat_score < threshold_lower || stat_score > threshold_upper;
    }
    return false;
}



// Function to iterate over an image and apply a callback for each pixel's values
void ImageProfile::iterateImage(const cv::Mat& img, const std::function<void(const std::vector<int>&)>& callback) {
    if (img.empty()) {
        throw std::runtime_error("Image is empty.");
    }

    int channels = img.channels();

    if (channels >= 1 && channels <= 4) {
        // Use cv::Vec with dynamic size based on the number of channels
        using VecType = cv::Vec<uchar, 4>;

        // Iterate over the image and extract values for each pixel
        for (auto it = img.begin<VecType>(); it != img.end<VecType>(); ++it) {
            const VecType& pixel = *it;

            // Collect only the relevant channel values based on the channel count
            std::vector<int> pixelValues(pixel.val, pixel.val + channels);

            // Apply the callback with the pixel values
            callback(pixelValues);
        }
    } else {
        throw std::runtime_error("Unsupported number of channels.");
    }
}

void ImageProfile::updatePixelValues(const std::vector<int>& pixelValues) {
     for (size_t i = 0; i < pixelValues.size(); ++i){
            pixelBox[i]->update(pixelValues[i]);
    }
}
