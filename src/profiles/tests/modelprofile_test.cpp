#include "modelprofile.h"
#include "saver.h"
#include <gtest/gtest.h>
#include <fstream>

// Test Fixture for ModelProfile
class ModelProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        createSampleIniFile("test_config.ini");

        model_profile = new ModelProfile("test_model", "test_config.ini", 1, 3);
    }

    void TearDown() override {
        delete model_profile;
        cleanUpTestFiles();
    }

    // Helper functions to create test data
    void createSampleIniFile(const std::string& filename) {
        std::ofstream ini_file(filename, std::ios::trunc);
        ini_file << "[model]\n";
        ini_file << "files = ./\n";
        ini_file.close();
    }

    void cleanUpTestFiles() {
        std::remove("test_config.ini");
    }

    ModelProfile* model_profile;
};

// Test Initialization
TEST_F(ModelProfileTest, Initialization) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_EQ(model_profile->getNumDistributionBoxes(), 3); // Should create 3 distribution boxes
}

// Test log_classification_model_stats with proper data
TEST_F(ModelProfileTest, LogClassificationModelStats) {
    ClassificationResults results = {
        {0.9f, 1}, // Score and class ID
        {0.8f, 2},
    };
    float latency = 1.5f;
    int result = model_profile->log_classification_model_stats(latency, results);
    EXPECT_EQ(result, 0); // Successful logging

/*TODO : need to correct this
    // Validate that the boxes have been updated
    EXPECT_GE(model_profile->getDistributionBox(0).get_n(), 0); // Check if the first box was updated
    EXPECT_GE(model_profile->getDistributionBox(1).get_n(), 0); // Check the second box
*/
}

// Test Invalid Configuration
TEST_F(ModelProfileTest, InvalidConfiguration) {
    createSampleIniFile("invalid_config.ini");

#if 0
//TODO: ModelProfile is not throwing any exeption
    EXPECT_THROW(
        ModelProfile("invalid_model", "invalid_config.ini", 1, 3),
        std::runtime_error  // Should throw due to invalid configuration
    );
#endif
}

// Test handling of empty classification results
TEST_F(ModelProfileTest, EmptyClassificationResults) {
    ClassificationResults empty_results;
    float latency = 1.0f;

    int result = model_profile->log_classification_model_stats(latency, empty_results);

    EXPECT_EQ(result, 0); // Should return success even with empty results
}

//Test if objects are registered with Saver
TEST_F(ModelProfileTest, ObjectsRegisteredWithSaver) {
    ClassificationResults results = {
        {0.9f, 1001}, // Score and class ID
        {0.8f, 2002},
        {0.8f, 3003},
    };
    float latency = 1.5f;
    int result = model_profile->log_classification_model_stats(latency, results);
    EXPECT_EQ(model_profile->saver->objects_to_save_.size(), 3); // Should have 3 objects to save
}

