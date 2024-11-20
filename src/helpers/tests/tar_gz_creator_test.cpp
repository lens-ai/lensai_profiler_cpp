#include <gtest/gtest.h>
#include "tar_gz_creator.h"
#include <fstream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class TarGzCreatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testFolder = "test_folder";
        tarFilePath = "test.tar";
        gzFilePath = "test.tar.gz";
        extractedFolder = "extracted";

        // Create test directory and files
        fs::create_directory(testFolder);
        std::ofstream(testFolder + "/file1.txt") << "Content of file 1";
        std::ofstream(testFolder + "/file2.txt") << "Content of file 2";
    }

    void TearDown() override {
        // Clean up created files and directories
        fs::remove_all(testFolder);
        fs::remove(tarFilePath);
        fs::remove(gzFilePath);
        fs::remove_all(extractedFolder);
    }

    std::string testFolder;
    std::string tarFilePath;
    std::string gzFilePath;
    std::string extractedFolder;
};

TEST_F(TarGzCreatorTest, CollectFilesFromFolders) {
    TarGzCreator creator;
    std::vector<std::string> folders = {testFolder};
    auto files = creator.collectFilesFromFolders(folders);

    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(fs::exists(files[0]));
    ASSERT_TRUE(fs::exists(files[1]));
}

TEST_F(TarGzCreatorTest, CreateTar) {
    TarGzCreator creator;

    auto files = creator.collectFilesFromFolders({testFolder});
    ASSERT_TRUE(creator.createTar(tarFilePath, files, testFolder));

    ASSERT_TRUE(fs::exists(tarFilePath));
}

TEST_F(TarGzCreatorTest, CompressToGz) {
    TarGzCreator creator;

    auto files = creator.collectFilesFromFolders({testFolder});
    ASSERT_TRUE(creator.createTar(tarFilePath, files, testFolder));
    ASSERT_TRUE(creator.compressToGz(tarFilePath, gzFilePath));

    ASSERT_TRUE(fs::exists(gzFilePath));
}

TEST_F(TarGzCreatorTest, DecompressGz) {
    TarGzCreator creator;

    auto files = creator.collectFilesFromFolders({testFolder});
    ASSERT_TRUE(creator.createTar(tarFilePath, files, testFolder));
    ASSERT_TRUE(creator.compressToGz(tarFilePath, gzFilePath));

    std::string decompressedTar = "decompressed.tar";
    ASSERT_TRUE(creator.decompressGz(gzFilePath, decompressedTar));

    ASSERT_TRUE(fs::exists(decompressedTar));
    fs::remove(decompressedTar);
}

TEST_F(TarGzCreatorTest, UnpackTar) {
    TarGzCreator creator;

    auto files = creator.collectFilesFromFolders({testFolder});
    ASSERT_TRUE(creator.createTar(tarFilePath, files, testFolder));

    ASSERT_TRUE(creator.unpackTar(tarFilePath, extractedFolder));

    ASSERT_TRUE(fs::exists(extractedFolder + "/file1.txt"));
    ASSERT_TRUE(fs::exists(extractedFolder + "/file2.txt"));
}

TEST_F(TarGzCreatorTest, EmptyFolder) {
    TarGzCreator creator;

    ASSERT_TRUE(creator.emptyFolder(testFolder));

    ASSERT_FALSE(fs::exists(testFolder + "/file1.txt"));
    ASSERT_FALSE(fs::exists(testFolder + "/file2.txt"));
}

