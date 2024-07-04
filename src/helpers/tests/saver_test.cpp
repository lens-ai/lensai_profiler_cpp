#include <gtest/gtest.h>
#include "saver.h" // Include your Saver header
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <kll_sketch.hpp>

typedef datasketches::kll_sketch<float> distributionBox;

// Test class with common test utilities
class SaverTest : public ::testing::Test {
protected:
    std::string testFilename = "test_save.dat"; // Name of the file to save

    void SetUp() override {
        // Clear the file before each test
        std::ofstream ofs(testFilename, std::ios::trunc);
        ofs.close();
    }

    void TearDown() override {
        // Remove the test file after each test
        std::remove(testFilename.c_str());
    }
};

TEST_F(SaverTest, AddObjectToSave) {
    Saver saver(5, "SaverTest"); // Save interval of 5 minutes
    distributionBox noiseBox;

    saver.AddObjectToSave((void*)(&noiseBox), KLL_TYPE, testFilename);
    // Check if the object was added to the queue
    EXPECT_TRUE(!saver.objects_to_save_.empty());
}

TEST_F(SaverTest, StartSavingAndTriggerSave) {
    Saver saver(5, "SaverTest"); // Save interval of 5 minutes
    distributionBox noiseBox;

    saver.StartSaving(); // Start the save loop
    saver.AddObjectToSave((void*)(&noiseBox), KLL_TYPE, testFilename);
    
    // Trigger save manually
    saver.TriggerSave();
    
    // Wait for some time to let the background thread work
    std::this_thread::sleep_for(std::chrono::seconds(2));
    saver.StopSaving();
    
    // Check if the file has the correct content
    std::ifstream is1(testFilename);
    auto sketch1 = datasketches::kll_sketch<float>::deserialize(is1);
  

//    EXPECT_EQ(u.get_min_item(), "42");
}

