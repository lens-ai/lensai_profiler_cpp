#ifndef TRACKER_PROFILE_H
#define TRACKER_PROFILE_H

#include <string>
#include <map>
#include "saver.h"
#include "generic.h"
#include "kll_sketch.hpp"

// Forward declaration of classes or structs if needed
class Saver;

// Define KLL sketch type
typedef datasketches::kll_sketch<float> distributionBox;

/**
 * @class TrackerProfile
 * @brief Class responsible for managing and logging tracking-related statistics.
 */
class TrackerProfile {
public:
    /**
     * @brief Constructor to initialize the TrackerProfile object.
     * @param conf_path Path to the configuration file.
     * @param save_interval Interval at which data is saved.
     */
    TrackerProfile(std::string conf_path, int save_interval);

    /**
     * @brief Destructor to clean up resources.
     */
    ~TrackerProfile();
    /**
     * @brief Log detection confidence to the confidence sketch.
     * @param confidence Detection confidence to be logged.
     */
    void log_detection_confidence(float confidence);

    /**
     * @brief Log track length to the track length sketch.
     * @param length Length of the track to be logged.
     */
    void log_track_length(int length);

    /**
     * @brief Log IoU to the IoU sketch.
     * @param iou IoU value to be logged.
     */
    void log_iou(float iou);

private:
    Saver* saver;  ///< Pointer to the saver object.
    std::map<std::string, std::string> modelConfig;  ///< Model configuration map.
    std::string statSavepath;  ///< Path to save statistics.

    // KLL sketches for tracking statistics
    distributionBox confidence_sketch_;
    distributionBox track_length_sketch_;
    distributionBox iou_sketch_;

    /**
     * @brief Register statistics to be saved.
     */
    void registerStatistics();

    // Configurations read from the INI file
    std::map<std::string, std::vector<std::string>> trackerConfig;

    /**
     * @brief Clean up dynamically allocated resources.
     */
    void cleanup();
};

#endif // TRACKER_PROFILE_H

