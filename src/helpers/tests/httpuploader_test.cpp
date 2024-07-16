#include "http_uploader.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

// Utility function to create a dummy gzip file for testing
void createDummyGzipFile(const std::string& filePath) {
    std::ofstream file(filePath, std::ios::binary);
    if (file.is_open()) {
        file << "dummy data";
        file.close();
    }
}

TEST(HttpUploaderTest, UploadFileSuccess) {
    std::string endpointUrl = "http://example.com/upload";
    std::string token = "dummy_token";
    std::string sensorId = "sensor_123";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "data";
    std::string filePath = "test_file.gz";

    // Create a dummy gzip file
    createDummyGzipFile(filePath);

    // Instantiate HttpUploader
    HttpUploader uploader(endpointUrl, token);

    // Test the postFile function
    bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);

    // Check that the file was successfully uploaded and deleted
    EXPECT_TRUE(result);
    EXPECT_FALSE(std::filesystem::exists(filePath));
}

TEST(HttpUploaderTest, UploadFileFailure) {
    std::string endpointUrl = "http://invalid-url/upload";
    std::string token = "dummy_token";
    std::string sensorId = "sensor_123";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "data";
    std::string filePath = "test_file.gz";

    // Create a dummy gzip file
    createDummyGzipFile(filePath);

    // Instantiate HttpUploader
    HttpUploader uploader(endpointUrl, token);

    // Test the postFile function
    bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);

    // Check that the file upload failed and the file still exists
    EXPECT_FALSE(result);
    EXPECT_TRUE(std::filesystem::exists(filePath));

    // Clean up
    std::filesystem::remove(filePath);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

