#include "http_uploader.h"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <thread> // For sleeping between tests

// Mock server URL from httpbin.org
const std::string mockServerUrl = "http://httpbin.org/post";
const std::string authToken = "your_auth_token_here"; // Replace with actual token if required

class HttpUploaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Optional: Any setup needed before each test
    }

    void TearDown() override {
        // Optional: Clean up after each test
    }
};

// Helper function to verify if a file exists
bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

// Mock function to simulate a successful file upload
bool mockUploadFunction(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType) {
    // Simulate HTTP POST request (replace with actual HTTP request)
    std::cout << "Uploading file: " << filePath << std::endl;
    std::cout << "Sensor ID: " << sensorId << ", Timestamp: " << timestamp << ", File Type: " << fileType << std::endl;

    // Simulate success/failure based on conditions
    bool success = true; // Simulate success

    // Simulate file deletion after successful upload
    if (success) {
        std::cout << "Deleting file: " << filePath << std::endl;
        std::remove(filePath.c_str()); // Delete the file
    }

    return success;
}

// Test case for successful upload and file deletion
TEST_F(HttpUploaderTest, SuccessfulUploadAndDelete) {
    std::string filePath = "test_data/sample_data.txt";
    std::string sensorId = "123456";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "txt";

    HttpUploader uploader(mockServerUrl, authToken);
    uploader.setUploadFunction(mockUploadFunction); // Set mock upload function

    bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);

    ASSERT_TRUE(result); // Check if upload was successful
    ASSERT_FALSE(fileExists(filePath)); // Check if file was deleted
}

// Test case for failed upload (simulate network failure)
TEST_F(HttpUploaderTest, FailedUpload) {
    std::string filePath = "test_data/sample_data.txt";
    std::string sensorId = "123456";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "txt";

    // Simulate network failure in mock upload function
    bool mockUploadFunctionFailed(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType) {
        std::cout << "Simulating network failure..." << std::endl;
        return false; // Simulate failure
    }

    HttpUploader uploader(mockServerUrl, authToken);
    uploader.setUploadFunction(mockUploadFunctionFailed); // Set mock upload function

    bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);

    ASSERT_FALSE(result); // Check if upload failed
    ASSERT_TRUE(fileExists(filePath)); // Check if file was not deleted
}

// Test case for invalid file path
TEST_F(HttpUploaderTest, InvalidFilePath) {
    std::string filePath = "invalid_path/file_not_exist.txt";
    std::string sensorId = "123456";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "txt";

    HttpUploader uploader(mockServerUrl, authToken);

    bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);

    ASSERT_FALSE(result); // Check if upload failed
    ASSERT_FALSE(fileExists(filePath)); // Check if file was not deleted
}

// Test case for multiple uploads
TEST_F(HttpUploaderTest, MultipleUploads) {
    std::vector<std::string> filePaths = { "test_data/sample_data1.txt", "test_data/sample_data2.txt" };
    std::string sensorId = "123456";
    time_t timestamp = std::time(nullptr);
    std::string fileType = "txt";

    HttpUploader uploader(mockServerUrl, authToken);
    uploader.setUploadFunction(mockUploadFunction); // Set mock upload function

    // Upload each file
    for (const auto& filePath : filePaths) {
        bool result = uploader.postFile(filePath, sensorId, timestamp, fileType);
        ASSERT_TRUE(result); // Check if upload was successful
        ASSERT_FALSE(fileExists(filePath)); // Check if file was deleted
    }
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

