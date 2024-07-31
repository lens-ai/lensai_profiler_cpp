#include <gtest/gtest.h>
#include "tar_gz_creator.h"
#include <boost/filesystem.hpp>
#include <libtar.h>
#include <fstream>

namespace fs = boost::filesystem;


// Test for collecting files from folders
TEST(TarGzCreatorTest, CollectFiles) {
    TarGzCreator creator;

    // Create a test folder with dummy files
    std::string testFolder = "test_folder";
    fs::create_directory(testFolder);

    std::ofstream(testFolder + "/file1.txt") << "File 1 content";
    std::ofstream(testFolder + "/file2.txt") << "File 2 content";

    std::vector<std::string> folders = {testFolder};
    std::vector<std::string> collectedFiles = creator.collectFilesFromFolders(folders);

    // Check that the collected files are as expected
    ASSERT_EQ(collectedFiles.size(), 2);
    ASSERT_TRUE(fs::exists(collectedFiles[0]));
    ASSERT_TRUE(fs::exists(collectedFiles[1]));

    // Cleanup test folder after test
    fs::remove_all(testFolder);
}

// Test for creating a tarball
TEST(TarGzCreatorTest, CreateTar) {
    TarGzCreator creator;

    // Create dummy files for tar
    std::string tarFilePath = "test.tar";
    std::string testFile1 = "test_file1.txt";
    std::string testFile2 = "test_file2.txt";

    std::ofstream(testFile1) << "Content of file 1";
    std::ofstream(testFile2) << "Content of file 2";

    std::vector<std::string> filesToTar = {testFile1, testFile2};

    ASSERT_TRUE(creator.createTar(tarFilePath, filesToTar));

    // Check that the tarball was created
    ASSERT_TRUE(fs::exists(tarFilePath));

    // Cleanup the created files and tarball after test
    std::remove(testFile1.c_str());
    std::remove(testFile2.c_str());
    fs::remove(tarFilePath);
}

// Test for compressing a tarball to gz
TEST(TarGzCreatorTest, CompressToGz) {
    TarGzCreator creator;

    // Create a tarball for testing
    std::string tarFilePath = "test.tar";
    std::string gzFilePath = "test.tar.gz";

    std::ofstream(tarFilePath) << "Dummy tarball content";

    ASSERT_TRUE(creator.compressToGz(tarFilePath, gzFilePath));

    // Check that the gzipped file was created
    ASSERT_TRUE(fs::exists(gzFilePath));

    // Cleanup created files after test
    fs::remove(tarFilePath);
    fs::remove(gzFilePath);
}

// Test case to decompress, unpack tarball, and verify file content
TEST(DecompressionAndUnpackTest, DecompressAndUnpack) {
    TarGzCreator creator;
    // Create gzipped tarball for testing
    std::string gzFilePath = "test.tar.gz";
    std::string tarFilePath = "test.tar";
    std::string outputFolderPath = "unpacked";

    // Create a tarball with sample data
    {
        std::ofstream tarFile(tarFilePath, std::ios::binary | std::ios::trunc);
        std::ofstream(tarFilePath + "/file1.txt") << "File 1 content";
        std::ofstream(tarFilePath + "/file2.txt") << "File 2 content";

        struct tar_header header = {};
        std::memset(&header, 0, sizeof(header));

        std::vector<std::string> filePaths = { "file1.txt", "file2.txt" };

        for (const auto& filePath : filePaths) {
            std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate);
            std::streamsize size = inputFile.tellg();
            inputFile.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            inputFile.read(buffer.data(), size);

            std::strncpy(header.name, filePath.c_str(), sizeof(header.name) - 1);
            snprintf(header.size, sizeof(header.size), "%011o", static_cast<int>(size));
            header.typeflag = '0';

            unsigned int checksum = 0;
            for (size_t i = 0; i < sizeof(header); ++i) {
                checksum += static_cast<unsigned char>(reinterpret_cast<char*>(&header)[i]);
            }
            snprintf(header.chksum, sizeof(header.chksum), "%06o", checksum);

            tarFile.write(reinterpret_cast<char*>(&header), sizeof(header));
            tarFile.write(buffer.data(), buffer.size());
        }

        std::vector<char> eof(1024, '\0');
        tarFile.write(eof.data(), eof.size());
    }

    // Compress the tarball to gz
    gzFilePath = "test.tar.gz";
    {
        gzFile gzOutput = gzopen(gzFilePath.c_str(), "wb");
        std::ifstream tarFile(tarFilePath, std::ios::binary);

        char buffer[4096]; // Buffer size for compression
        while (tarFile.read(buffer, sizeof(buffer))) {
            gzwrite(gzOutput, buffer, tarFile.gcount());
        }

        gzclose(gzOutput);
        tarFile.close();
    }

    // Test decompression
    std::string decompressedFilePath = "decompressed.tar";
    ASSERT_TRUE(creator.decompressGz(gzFilePath, decompressedFilePath));

    // Test unpacking tarball
    ASSERT_TRUE(creator.unpackTar(decompressedFilePath, outputFolderPath));

    // Verify content of extracted files
    std::string extractedFile1 = outputFolderPath + "/file1.txt";
    std::string extractedFile2 = outputFolderPath + "/file2.txt";

    std::ifstream extracted1(extractedFile1);
    std::string content1((std::istreambuf_iterator<char>(extracted1)), std::istreambuf_iterator<char>());
    ASSERT_EQ(content1, "File 1 content");

    std::ifstream extracted2(extractedFile2);
    std::string content2((std::istreambuf_iterator<char>(extracted2)), std::istreambuf_iterator<char>());
    ASSERT_EQ(content2, "File 2 content");

    // Cleanup test files and folders
    fs::remove_all(outputFolderPath);
    fs::remove(decompressedFilePath);
    fs::remove(tarFilePath);
    fs::remove(gzFilePath);
}
