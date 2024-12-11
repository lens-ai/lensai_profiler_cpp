#ifndef TRACKING_PROFILE_H
#define TRACKING_PROFILE_H

#include <string>
#include <map>
#include "saver.h"
#include "generic.h"
#include "kll_sketch.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <Eigen/Dense>  // For matrix operations and quaternion support

// Forward declaration of classes or structs if needed
class Saver;

// Define KLL sketch type
typedef datasketches::kll_sketch<float> distributionBox;

// 2D and 3D position structs
struct Position2D {
    float x, y;
};

struct Position3D {
    float x, y, z;
};

// Quaternion struct
struct Quaternion {
    float w, x, y, z;
};

// Angular Velocity struct (in radians/second)
struct AngularVelocity {
    float roll_rate, pitch_rate, yaw_rate;
};

/**
 * @class TrackerProfile
 * @brief Class responsible for managing and logging tracking-related statistics.
 */
class TrackingProfile {
public:
    /**
     * @brief Constructor to initialize the TrackerProfile object.
     * @param conf_path Path to the configuration file.
     * @param save_interval Interval at which data is saved.
     */
    TrackingProfile(std::string conf_path, int save_interval);

    /**
     * @brief Destructor to clean up resources.
     */
    ~TrackingProfile();
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
    void log_orientation_error(float orientation_error); 
    void log_angular_velocity_latency(float angular_velocity_latency);
    // Methods for Data Sketches (KLL Sketch)
    void log_position_error(float position_error);
    void log_orientation_error(const Quaternion& orientation);
    void log_angular_velocity_latency(const AngularVelocity& angular_velocity);
    void log_quaternion_drift(float quaternion_drift);
    void log_covariance_spread(float covariance_spread);
    void log_anomalous_rotation(float anomalous_rotation);
    void log_angular_divergence(float angular_divergance);
private:
    Saver* saver;  ///< Pointer to the saver object.
    std::map<std::string, std::string> modelConfig;  ///< Model configuration map.
    std::string statSavepath;  ///< Path to save statistics.

    // KLL sketches for tracking statistics
    distributionBox confidence_sketch_;
    distributionBox track_length_sketch_;
    distributionBox iou_sketch_;
    distributionBox positionError_sketch;
    distributionBox orientationError_sketch;
    distributionBox angularVelocityLatency_sketch;
    distributionBox covarianceSpread_sketch;
    distributionBox angularDivergence_sketch;
    distributionBox anomalousRotation_sketch;
    distributionBox quaternionDrift_sketch;

    float positionError2D, positionError3D;
    float orientationError;
    float angularVelocityLatency;
    float quaternionDrift;
    float covarianceSpread;
    float angularDivergence;
    bool anomalousRotation;

    /**
     * @brief Register statistics to be saved.
     */
    void registerStatistics(const std::map<std::string, std::vector<std::string>> trackerConfig);
    // Configurations read from the INI file
    std::map<std::string, std::vector<std::string>> trackerConfig;

    /**
     * @brief Clean up dynamically allocated resources.
     */
    void cleanup();
};

#endif // TRACKING_PROFILE_H
