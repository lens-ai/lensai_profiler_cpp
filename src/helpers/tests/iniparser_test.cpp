#include <gtest/gtest.h>
#include "iniparser.h" // Assuming this is the header where the parseIniFile method is defined
#include <map>
#include <fstream>


// A utility function to create a temporary INI file for testing
void createTestIniFile(const std::string& filename, const std::string& content) {
    std::ofstream outFile(filename);
    outFile << content;
    outFile.close();
}

class IniParserTest : public ::testing::Test {
protected:
    std::string testIniFilename = "test.ini";

    virtual void SetUp() override {
        // Creating a basic test INI file with sections and subsections
        std::string iniContent = R"(
[sampling]
marginconfidence = 0.5
least = 0.4
[imagemetrics]
noise = 0.5
)";

        createTestIniFile(testIniFilename, iniContent);
    }

    virtual void TearDown() override {
        // Clean up the test INI file after tests
        std::remove(testIniFilename.c_str());
    }
};

TEST_F(IniParserTest, ParseSpecificKey) {
    IniParser parser;
    auto result = parser.parseIniFile(testIniFilename, "sampling", "marginconfidence");

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result["marginconfidence"], "0.5");
}

TEST_F(IniParserTest, ParseAllKeysInSection) {
    IniParser parser;
    auto result = parser.parseIniFile(testIniFilename, "sampling", "");

    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result["marginconfidence"], "0.5");
    ASSERT_EQ(result["least"], "0.4");
}

TEST_F(IniParserTest, ParseInvalidKey) {
    IniParser parser;
    auto result = parser.parseIniFile(testIniFilename, "sampling", "nonexistent_key");

    ASSERT_EQ(result.size(), 0);
}

TEST_F(IniParserTest, ParseInvalidSection) {
    IniParser parser;
    auto result = parser.parseIniFile(testIniFilename, "nonexistent_section", "");

    ASSERT_EQ(result.size(), 0);
}

TEST_F(IniParserTest, ParseEmptyIniFile) {
    createTestIniFile(testIniFilename, ""); // Overwriting with an empty content
    std::map<std::string, std::string> result = IniParser::parseIniFile(testIniFilename, "anysection", "");
    EXPECT_TRUE(result.empty()); // The result should be empty
}

