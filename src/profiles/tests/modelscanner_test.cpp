#include "ModelScanner.h"
#include <gtest/gtest.h>
#include <tensorflow/lite/model.h>
#include <torch/script.h>
#include <fstream>
#include <iostream>

// Helper function to create a sample INI file for testing
void createSampleIniFile(const std::string& filePath) {
    std::ofstream iniFile(filePath);
    iniFile << "[Operators]" << std::endl;
    iniFile << "ADD=1" << std::endl;
    iniFile << "CONV_2D=1" << std::endl;
    iniFile << "[Metadata]" << std::endl;
    iniFile << "author=Test" << std::endl;
    iniFile.close();
}

// Test fixture for TensorFlow Lite model scanning
class TensorFlowLiteModelScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        iniFilePath = "test_config.ini";
        createSampleIniFile(iniFilePath);
    }

    void TearDown() override {
        std::remove(iniFilePath.c_str());
    }

    std::string iniFilePath;
};

TEST_F(TensorFlowLiteModelScannerTest, GenerateExpectedData) {
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile("test_model.tflite");
    ASSERT_NE(model, nullptr) << "Failed to load test model.";

    EXPECT_TRUE(ModelScanner::generateExpectedData(model.get(), iniFilePath))
        << "Failed to generate expected data.";
}

TEST_F(TensorFlowLiteModelScannerTest, ValidateMetadata) {
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile("test_model.tflite");
    ASSERT_NE(model, nullptr) << "Failed to load test model.";

    EXPECT_TRUE(ModelScanner::validateMetadata(model.get(), iniFilePath))
        << "Metadata validation failed.";
}

TEST_F(TensorFlowLiteModelScannerTest, ValidateOperators) {
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile("test_model.tflite");
    ASSERT_NE(model, nullptr) << "Failed to load test model.";

    EXPECT_TRUE(ModelScanner::validateOperators(model.get(), iniFilePath))
        << "Operator validation failed.";
}

TEST_F(TensorFlowLiteModelScannerTest, ValidateTensorBuffers) {
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile("test_model.tflite");
    ASSERT_NE(model, nullptr) << "Failed to load test model.";

    EXPECT_TRUE(ModelScanner::validateTensorBuffers(model.get()))
        << "Tensor buffer validation failed.";
}

// Test fixture for PyTorch model scanning
class PyTorchModelScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        iniFilePath = "test_config.ini";
        createSampleIniFile(iniFilePath);
    }

    void TearDown() override {
        std::remove(iniFilePath.c_str());
    }

    std::string iniFilePath;
};

TEST_F(PyTorchModelScannerTest, ValidatePyTorchOperators) {
    torch::jit::script::Module model;
    try {
        model = torch::jit::load("test_model.pt");
    } catch (const c10::Error& e) {
        FAIL() << "Failed to load test PyTorch model.";
    }

    EXPECT_TRUE(ModelScanner::validatePyTorchOperators(model, iniFilePath))
        << "PyTorch operator validation failed.";
}

// Main function to run all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

