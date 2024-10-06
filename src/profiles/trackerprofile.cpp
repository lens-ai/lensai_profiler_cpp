#include "trackerprofile.h"
#include "iniparser.h"
#include <stdexcept>  // For exception handling
#include <iostream>   // For error reporting

// Initialize the logger and sketches
TrackerProfile::TrackerProfile(std::string conf_path, int save_interval) 
    : saver(nullptr) {  // Initialize saver to nullptr for safety
    try {
        // Initialize saver
        saver = new Saver(save_interval, "TrackerProfile");

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
        registerStatistics();

        // Start saving process
        saver->StartSaving();
    } catch (const std::exception& e) {
        std::cerr << "TrackerProfile Initialization Failed: " << e.what() << std::endl;
        cleanup();  // Call cleanup to release resources in case of failure
        throw;      // Re-throw the exception after cleanup
    }
}

TrackerProfile::~TrackerProfile() {
    cleanup();  // Ensure cleanup of dynamic memory
}

// Function to clean up dynamic memory
void TrackerProfile::cleanup() {
    if (saver) {
        delete saver;
        saver = nullptr;  // Set to nullptr after deletion
    }
}

// Register Model embeddings saver
void TrackerProfile::registerStatistics() {
    try {
        saver->AddObjectToSave((void*)(&confidence_sketch_), KLL_TYPE, statSavepath + "track_confidence.bin");
        saver->AddObjectToSave((void*)(&track_length_sketch_), KLL_TYPE, statSavepath + "track_length.bin");
        saver->AddObjectToSave((void*)(&iou_sketch_), KLL_TYPE, statSavepath + "track_iou.bin");
    } catch (const std::exception& e) {
        std::cerr << "Failed to register statistics: " << e.what() << std::endl;
        throw;  // Re-throw exception to signal failure in initialization
    }
}

void TrackerProfile::log_detection_confidence(float confidence) {
    try {
        confidence_sketch_.update(confidence);
    } catch (const std::exception& e) {
        std::cerr << "Error logging detection confidence: " << e.what() << std::endl;
    }
}

void TrackerProfile::log_track_length(int length) {
    try {
        track_length_sketch_.update(length);
    } catch (const std::exception& e) {
        std::cerr << "Error logging track length: " << e.what() << std::endl;
    }
}

void TrackerProfile::log_iou(float iou) {
    try {
        iou_sketch_.update(iou);
    } catch (const std::exception& e) {
        std::cerr << "Error logging IoU: " << e.what() << std::endl;
    }
}

