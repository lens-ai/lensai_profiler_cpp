/**
 * @file ImageProfile.cpp
 * @brief Implements ImageProfile class for image analysis
 */

#include <vector>
#include <cmath>
#include "customprofile.h"
#include <iniparser.h>

CustomProfile::~CustomProfile() {
    delete saver;
}


/**
 * @class CustomProfile
 * @brief Class for computing and managing various image statistics
 * @brief Constructor to initialize ImageProfile object
 * @param conf_path Path to configuration file
 * @param save_interval Interval for saving statistics
 */
CustomProfile::CustomProfile(const std::string& conf_path, int save_interval)
    : saver(new Saver(save_interval, "CustomProfile")) {
    try {
        // Read configuration settings
        IniParser parser;
	customConfig = parser.parseIniFile(conf_path, "custom", "");
	statSavepath = customConfig["filepath"][0];
	dataSavepath = 	customConfig["filepath"][1];
	std::cout << statSavepath << dataSavepath << std::endl;
        createFolderIfNotExists(statSavepath, dataSavepath);
        customConfig.erase("filepath");

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
int CustomProfile::profile(const std::string& name, float value) {
    // Get the distributionBox for the given statistic name
    distributionBox* custom_dBox = getBox(name);    

    // Update the distributionBox with the given value
    custom_dBox->update(value);

    return 1; // Indicate success
}


/**
 * @brief Gets the distributionBox (KLL sketch) for the given statistic name.
 * If it doesn't exist, a new one is created and added to the map.
 * @param name Statistic name
 * @return Pointer to the distributionBox object
 */
distributionBox* CustomProfile::getBox(const std::string& name) {
    // Convert the name to a unique integer ID, for example, by hashing
    int stat_id = std::hash<std::string>{}(name);

    // Check if the distributionBox exists in the map
    if (custom_stat_.find(stat_id) == custom_stat_.end()) {
        // If not, create a new distributionBox and add it to the map
        custom_stat_[stat_id] = new distributionBox();
        
        // Also register the new box for saving
        saver->AddObjectToSave((void*)(custom_stat_[stat_id]), KLL_TYPE, statSavepath + name + ".bin");
    }

    // Return the distributionBox
    return custom_stat_[stat_id];
}

