#include "trackingprofile.h"
#include "iniparser.h"
#include <stdexcept>  // For exception handling
#include <iostream>   // For error reporting
#include <cmath>
#include <iostream>
#include <Eigen/Dense>


// Initialize the logger and sketches
TrackingProfile::TrackingProfile(std::string conf_path, int save_interval) 
    : saver(nullptr){  // Initialize saver to nullptr for safety
    try {
        // Initialize saver
        saver = new Saver(save_interval, "TrackingProfile");

        // Parse configuration
        IniParser parser;
        trackerConfig = parser.parseIniFile(conf_path, "tracker", "");
        
        // Get the save path from configuration
        if (trackerConfig["filepath"].empty()) {
            throw std::runtime_error("Error: No filepath found in configuration.");
        }
        statSavepath = trackerConfig["filepath"][0];
        createFolder(statSavepath);

        // Register statistics for saving
        registerStatistics(trackerConfig);

        // Start saving process
        saver->StartSaving();
    } catch (const std::exception& e) {
        std::cerr << "TrackingProfile Initialization Failed: " << e.what() << std::endl;
        cleanup();  // Call cleanup to release resources in case of failure
        throw;      // Re-throw the exception after cleanup
    }
}

TrackingProfile::~TrackingProfile() {
    cleanup();  // Ensure cleanup of dynamic memory
}

// Function to clean up dynamic memory
void TrackingProfile::cleanup() {
    if (saver) {
        delete saver;
        saver = nullptr;  // Set to nullptr after deletion
    }
}

// Register Model embeddings saver
void TrackingProfile::registerStatistics(std::map<std::string, std::vector<std::string>> trackerConfig){
    try {
	if (trackerConfig["DETECTION_CONFIDENCE"][0] == "true"){ 
	    saver->AddObjectToSave((void*)(&confidence_sketch_), KLL_TYPE, statSavepath + "track_confidence.bin");
	}
        if (trackerConfig["TRACK_LENGTH"][0] == "true"){ 
	    saver->AddObjectToSave((void*)(&track_length_sketch_), KLL_TYPE, statSavepath + "track_length.bin");
	}
	if (trackerConfig["TRACK_IOU"][0] == "true"){
	   saver->AddObjectToSave((void*)(&iou_sketch_), KLL_TYPE, statSavepath + "track_iou.bin");
	}
	if (trackerConfig["POSITION_ERROR"][0] == "true"){ 
	    saver->AddObjectToSave((void*)(&positionError_sketch), KLL_TYPE, statSavepath + "position_error.bin");
	}
        if (trackerConfig["ORIENTATION_ERROR"][0] == "true"){
	    saver->AddObjectToSave((void*)(&orientationError_sketch), KLL_TYPE, statSavepath + "orientation_error.bin");
	}
	if (trackerConfig["ANGULAR_VELOCITY_LATENCY"][0] == "true"){
	   saver->AddObjectToSave((void*)(&angularVelocityLatency_sketch), KLL_TYPE, statSavepath + "angular_velocity_latency.bin");
	}
	if (trackerConfig["COVARIANCE_SPREAD"][0] == "true"){ 
	   saver->AddObjectToSave((void*)(&covarianceSpread_sketch), KLL_TYPE, statSavepath + "covariance_spread.bin");
	}
        if (trackerConfig["ANGULAR_DIVERGENCE"][0] == "true"){
	   saver->AddObjectToSave((void*)(&angularDivergence_sketch), KLL_TYPE, statSavepath + "angular_divergence.bin");
	}
	if (trackerConfig["ANOMALOUS_ROTATION"][0] == "true"){ 
	   saver->AddObjectToSave((void*)(&anomalousRotation_sketch), KLL_TYPE, statSavepath + "anomalous_rotation.bin");
	}
        if (trackerConfig["QUATERNION_DRIFT"][0] == "true"){
	   saver->AddObjectToSave((void*)(&quaternionDrift_sketch), KLL_TYPE, statSavepath + "quaternion_drift.bin");
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to register statistics: " << e.what() << std::endl;
        throw;  // Re-throw exception to signal failure in initialization
    }
}

void TrackingProfile::log_detection_confidence(float confidence) {
    try {
        confidence_sketch_.update(confidence);
    } catch (const std::exception& e) {
        std::cerr << "Error logging detection confidence: " << e.what() << std::endl;
    }
}

void TrackingProfile::log_track_length(int length) {
    try {
        track_length_sketch_.update(length);
    } catch (const std::exception& e) {
        std::cerr << "Error logging track length: " << e.what() << std::endl;
    }
}

void TrackingProfile::log_iou(float iou) {
    try {
        iou_sketch_.update(iou);
    } catch (const std::exception& e) {
        std::cerr << "Error logging IoU: " << e.what() << std::endl;
    }
}

// Update Position Sketches (2D and 3D)
void TrackingProfile::log_position_error(float position_error){ 
    try {	
        positionError_sketch.update(position_error);
    } catch (const std::exception& e) {
        std::cerr << "Error logging position error: " << e.what() << std::endl;
    }
}

void TrackingProfile::log_orientation_error(float orientation_error) {
    try {	
        orientationError_sketch.update(orientation_error);
    } catch (const std::exception& e) {
	    std::cerr << "Error logging orientation error" << e.what() << std::endl;
    }
}

// Update Angular Velocity Sketch
void TrackingProfile::log_angular_velocity_latency(float angular_velocity_latency) {
    try {	
    angularVelocityLatency_sketch.update(angular_velocity_latency);
    } catch (const std::exception& e) {
        std::cerr << "Error logging angular velocity latency" << e.what() << std::endl;
    }
}

void TrackingProfile::log_quaternion_drift(float quaternion_drift){
   try {
       quaternionDrift_sketch.update(quaternion_drift);
   } catch (const std::exception& e) {
       std::cerr << "Error logging quaternion drift" << e.what() << std::endl;
   } 
}

void TrackingProfile::log_covariance_spread(float covariance_spread){
  try {	
      covarianceSpread_sketch.update(covariance_spread);
  } catch (const std::exception& e) {
      std::cerr << "Error Logging covariance spread " << e.what() << std::endl;
  } 
}

void TrackingProfile::log_anomalous_rotation(float anomalous_rotation){
  try {	
      anomalousRotation_sketch.update(anomalous_rotation);
  } catch (const std::exception& e) {
      std::cerr << "Error logging anomaous rotation" << e.what() << std::endl;
  }    
}

void TrackingProfile::log_angular_divergence(float angular_divergence){
  try {	
      angularDivergence_sketch.update(angular_divergence);
  } catch (const std::exception& e) {
      std::cerr << "Error logging angular divergence " << e.what() << std::endl;
  }  
}
