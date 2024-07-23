#include "tar_gz_creator.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

TarGzCreator::TarGzCreator() {}

std::vector<std::string> TarGzCreator::collectFilesFromFolders(const std::vector<std::string>& folders) {
    std::vector<std::string> collectedFiles;

    for (const auto& folder : folders) {
        if (fs::exists(folder) && fs::is_directory(folder)) {
            for (const auto& entry : fs::recursive_directory_iterator(folder)) {
                if (fs::is_regular_file(entry.path())) {
                    collectedFiles.push_back(entry.path().string());
                }
            }
        } else {
            std::cerr << "Directory not found: " << folder << std::endl;
        }
    }

    return collectedFiles;
}

void TarGzCreator::addPadding(std::ofstream& stream, std::streamsize size) {
    std::streamsize padding = 512 - (size % 512);
    if (padding != 512) {
        std::vector<char> paddingBuffer(padding, '\0');
        stream.write(paddingBuffer.data(), paddingBuffer.size());
    }
}

bool TarGzCreator::createTar(const std::string& tarFilePath, const std::vector<std::string>& filePaths) {
    std::ofstream tarFile(tarFilePath, std::ios::binary | std::ios::trunc);

    if (!tarFile.is_open()) {
        return false;
    }

    for (const auto& filePath : filePaths) {
        std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate);

        if (!inputFile.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            continue;
        }

        std::streamsize size = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        inputFile.read(buffer.data(), size);

        struct tar_header header = {};
        std::memset(&header, 0, sizeof(header));

        std::string relativePath = fs::relative(filePath).string();
        std::strncpy(header.name, relativePath.c_str(), sizeof(header.name) - 1);

        snprintf(header.size, sizeof(header.size), "%011o", static_cast<int>(size));
        snprintf(header.mode, sizeof(header.mode), "%07o", 0644);
        snprintf(header.mtime, sizeof(header.mtime), "%011ld", std::time(nullptr));
        header.typeflag = '0';

        unsigned int checksum = 0;
        for (size_t i = 0; i < sizeof(header); ++i) {
            checksum += static_cast<unsigned char>(reinterpret_cast<char*>(&header)[i]);
        }
        snprintf(header.chksum, sizeof(header.chksum), "%06o", checksum);

        tarFile.write(reinterpret_cast<char*>(&header), sizeof(header));
        tarFile.write(buffer.data(), buffer.size());

        addPadding(tarFile, size);
    }

    std::vector<char> eof(1024, '\0');
    tarFile.write(eof.data(), eof.size());

    tarFile.close();

    return true;
}

bool TarGzCreator::compressToGz(const std::string& tarFilePath, const std::string& gzFilePath) {
    std::ifstream tarFile(tarFilePath, std::ios::binary | std::ios::ate);
    gzFile gzOutput = gzopen(gzFilePath.c_str(), "wb");

    if (!tarFile.is_open() || !gzOutput) {
        return false;
    }

    std::streamsize size = tarFile.tellg();
    tarFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    tarFile.read(buffer.data(), size);

    gzwrite(gzOutput, buffer.data(), buffer.size());

    gzclose(gzOutput);
    tarFile.close();

    return true;
}

// Function to decompress a gz file
bool TarGzCreator::decompressGz(const std::string& gzFilePath, const std::string& outputFilePath) {
    gzFile gzInput = gzopen(gzFilePath.c_str(), "rb");
    if (!gzInput) {
        return false;
    }

    std::ofstream outputFile(outputFilePath, std::ios::binary | std::ios::trunc);
    if (!outputFile.is_open()) {
        gzclose(gzInput);
        return false;
    }

    char buffer[4096]; // Buffer size for decompression
    int bytesRead;
    while ((bytesRead = gzread(gzInput, buffer, sizeof(buffer))) > 0) {
        outputFile.write(buffer, bytesRead);
    }

    gzclose(gzInput);
    outputFile.close();

    return true;
}

// Function to unpack a tar archive
bool TarGzCreator::unpackTar(const std::string& tarFilePath, const std::string& outputFolderPath) {
    std::ifstream tarFile(tarFilePath, std::ios::binary);
    if (!tarFile.is_open()) {
        return false;
    }

    while (tarFile) {
        struct tar_header header = {};
        tarFile.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (tarFile.gcount() == 0) {
            break; // End of tarball
        }

        // Get the file size from the tar header
        std::streamsize size = std::stoul(std::string(header.size), nullptr, 8);

        std::string relativeFilePath(header.name);
        std::string outputFilePath = outputFolderPath + "/" + relativeFilePath;

        // Create directories as needed
        fs::create_directories(fs::path(outputFilePath).parent_path());

        std::ofstream outputFile(outputFilePath, std::ios::binary | std::ios::trunc);
        if (!outputFile.is_open()) {
            return false;
        }

        std::vector<char> buffer(size);
        tarFile.read(buffer.data(), size);
        outputFile.write(buffer.data(), buffer.size());

        outputFile.close();

        // Skip padding to align with 512-byte blocks
        tarFile.seekg((512 - (size % 512)) % 512, std::ios::cur);
    }

    tarFile.close();

    return true;
}

bool TarGzCreator::emptyFolder(const std::string& folderPath) {
    std::lock_guard<std::mutex> lock(folderMutex); // Lock the folder access

    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry.path())) {
                fs::remove(entry.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error emptying folder: " << e.what() << std::endl;
        return false;
    }

    return true;
}
