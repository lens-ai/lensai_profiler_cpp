#include "imagesampler.h"
#include "saver.h"
#include <gtest/gtest.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <fstream>

// Test fixture for ImageSampler
class ImageSamplerTest : public ::testing::Test {
protected:
    void SetUp() override {
        createSampleIniFile("test_config.ini");  // Create a sample configuration file
        sampler = new ImageSampler("test_config.ini", 1);  // Initialize ImageSampler
    }

    void TearDown() override {
        delete sampler;
        cleanUpTestFiles();  // Clean up test files
    }

    // Helper function to create a sample INI file
    void createSampleIniFile(const std::string& filename) {
        std::ofstream ini_file(filename, std::ios::trunc);
        ini_file << "[sampling]\n";
        ini_file << "filepath = ./\n";
        ini_file << "MARGINCONFIDENCE = 0.1\n";  // Sample threshold for margin confidence
        ini_file << "LEASTCONFIDENCE = 0.2\n";
        ini_file << "RATIOCONFIDENCE = 0.3\n";
        ini_file << "ENTROPYCONFIDENCE = 0.4\n";
        ini_file.close();
    }

    // Helper function to clean up test files
    void cleanUpTestFiles() {
        std::remove("test_config.ini");  // Remove the test INI file
        std::remove("savefile.dat");  // Remove the test save file
    }

    ImageSampler* sampler;
};

// Test Initialization
TEST_F(ImageSamplerTest, Initialization) {
    EXPECT_EQ(sampler->filesSavePath, "./");  // Expected file path
}

// Test margin_confidence
TEST_F(ImageSamplerTest, MarginConfidence) {
    std::vector<float> prob_dist = {0.7f, 0.5f, 0.2f};  // Sample probabilities
    float result = sampler->margin_confidence(prob_dist, false);  // Not sorted
    EXPECT_FLOAT_EQ(result, 0.8f);  // Expected margin confidence score
}

// Test least_confidence
TEST_F(ImageSamplerTest, LeastConfidence) {
    std::vector<float> prob_dist = {0.7f, 0.5f, 0.2f};  // Sample probabilities
    float result = sampler->least_confidence(prob_dist, false);
    EXPECT_NEAR(result, 0.7f, 0.5);  // Expected least confidence score
}

// Test ratio_confidence
TEST_F(ImageSamplerTest, RatioConfidence) {
    std::vector<float> prob_dist = {0.7f, 0.5f, 0.2f};  // Sample probabilities
    float result = sampler->ratio_confidence(prob_dist, false);
    EXPECT_NEAR(result, 0.714f, 0.5);  // Expected ratio confidence score (0.5 / 0.7)
}

// Test entropy_confidence
TEST_F(ImageSamplerTest, EntropyConfidence) {
    std::vector<float> prob_dist = {0.7f, 0.5f, 0.2f};  // Sample probabilities
    float result = sampler->entropy_confidence(prob_dist);
    EXPECT_GT(result, 0);  // Entropy should be greater than 0
}

// Test the sample method
TEST_F(ImageSamplerTest, SampleMethod) {
    std::vector<std::pair<float, int>> classificationResults = {
        {0.7f, 1},  // Confidence score and class ID
        {0.5f, 2},
        {0.2f, 3},
    };

    cv::Mat img = cv::Mat::ones(100, 100, CV_8UC1) * 128;  // Simple grayscale image
    int result = sampler->sample(classificationResults, img, true);  // Sample with save_sample = true
    EXPECT_EQ(result, 1);  // Expected success
}
