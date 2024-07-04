// test_class.cpp

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "imghelpers.h" // Replace with your class header file

class ImageProcessingTest : public ::testing::Test {
protected:
    cv::Mat colorImage;
    cv::Mat grayscaleImage;
    std::string testImagePath = "test_images";
    std::string testImageBaseName = "test_image";

    void SetUp() override {
        colorImage = cv::Mat(100, 100, CV_8UC3, cv::Scalar(100, 150, 200)); // A simple 100x100 image
        grayscaleImage = cv::Mat(100, 100, CV_8UC1, cv::Scalar(127)); // A grayscale image
    }
};

// Test convertGrayScale function
TEST_F(ImageProcessingTest, convertGrayScale) {
    cv::Mat grayscaleResult;
    int result = convertGrayScale(colorImage, grayscaleResult);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(grayscaleResult.channels(), 1); // Grayscale should have one channel
}

// Test calcSharpness function
TEST_F(ImageProcessingTest, calculateSharpnessLaplacian) {
    double sharpness = calculateSharpnessLaplacian(grayscaleImage);
    EXPECT_GE(sharpness, 0); // Sharpness should be greater than zero
}

// Test calcSNR function
TEST_F(ImageProcessingTest, calculateSNR) {
    double snr = calculateSNR(grayscaleImage);
    EXPECT_GT(snr, 0); // SNR should be greater than zero
}

// Test calcContrast function
TEST_F(ImageProcessingTest, calculateContrast) {
    double contrast = calculateContrast(grayscaleImage);
    EXPECT_GE(contrast, 0); // Contrast should not be negative
}

// Test saveImageWithIncrementalName function
TEST_F(ImageProcessingTest, SaveImageWithIncrementalName) {
    std::string savedImagePath = saveImageWithIncrementalName(colorImage, testImagePath, testImageBaseName);
    ASSERT_FALSE(savedImagePath.empty()); // Path should not be empty
    EXPECT_TRUE(cv::imread(savedImagePath).data != nullptr); // The image should be readable
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

