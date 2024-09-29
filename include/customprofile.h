#ifndef CUSTOMPROFILE_H
#define CUSTOMPROFILE_H

#include <string>
#include <unordered_map>
#include <opencv2/opencv.hpp>  // For OpenCV matrix types
#include <iostream>
#include <vector>
#include <map>
#include "saver.h"
#include "generic.h"
#include <kll_sketch.hpp>


/**
 * @brief Class for managing and logging model statistics
 */
typedef datasketches::kll_sketch<float> distributionBox;

class CustomProfile {
public:
    /**
     * @brief Constructor to initialize CustomProfile object
     * @param conf_path Path to configuration file
     * @param save_interval Interval for saving statistics
     * @param channels Number of channels in the image
     */
    CustomProfile(const std::string& conf_path, int save_interval);
    /**
     * @brief Destructor to clean up resources
     */
    ~CustomProfile();

    /**
     * @brief Computes and logs selected statistics
     * @param name Name of the statistic to profile
     * @param value Value to update in the statistic
     * @return 1 on success, error code on failure
     */
    int profile(const std::string& name, float value);

private:
    /**
     * @brief Registers statistics for saving based on configuration
     * @param name Statistic name
     */
    void registerStatistics(const std::string& name);

    /**
     * @brief Gets the distributionBox (KLL sketch) for the given statistic name.
     * If it doesn't exist, a new one is created and added to the map.
     * @param name Statistic name
     * @return Pointer to the distributionBox object
     */
    distributionBox* getBox(const std::string& name);

    // Configurations read from the INI file
    std::map<std::string, std::vector<std::string>> customConfig;

    // Path for saving statistics
    std::string statSavepath;
    std::string dataSavepath;

    // Saver object for saving statistics periodically
    Saver* saver;

    // Map to store KLL sketches based on statistic names
    std::unordered_map<int, distributionBox*> custom_stat_;

};

#endif // CUSTOMPROFILE_H

