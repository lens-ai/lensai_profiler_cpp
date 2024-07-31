#include "tar_gz_creator.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <filesystem>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

TarGzCreator::TarGzCreator() {}

void TarGzCreator::add_file_to_tar(TAR *tar, const std::string& path,
                                    const std::string& basePath) {
    std::string path_in_tar;
    size_t pos = path.find(basePath);

    std::cout << "add_file_to_tar : " << path << " : " << basePath << std::endl;

    if (pos != std::string::npos) {
        path_in_tar = path.substr(pos+basePath.length());
    } else {
        path_in_tar = path;
    }
 
    if (tar_append_file(tar, path.c_str(), path_in_tar.c_str()) == -1) {
        perror("tar_append_file");
        return;
    }
}

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

bool TarGzCreator::createTar(const std::string& tarFilePath, const std::vector<std::string>& filePaths, const std::string& basePath) {
    TAR *tar;
    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    // Open the TAR file for writing
    if (tar_open(&tar, tarFilePath.c_str(), NULL, flags, mode, TAR_GNU) == -1) {
        perror("tar_open");
        return false;
    }

    // Add each file to the TAR archive
    for (const auto& filePath : filePaths) {
        if (filePath.find("_lock") == std::string::npos)
            add_file_to_tar(tar, filePath, basePath);
    }

    // Close the TAR file
    if (tar_close(tar) == -1) {
        perror("tar_close");
        return false;
    }

    std::cout << "Files added to " << tarFilePath << " successfully." << std::endl;
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
    std::cout << "compressToGz : " << size << std::endl;

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
    return true;
}

bool TarGzCreator::emptyFolder(const std::string& folderPath) {
    std::lock_guard<std::mutex> lock(folderMutex); // Lock the folder access

    try {
        if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
            for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
                if (fs::is_regular_file(entry.path())) {
                    fs::remove(entry.path());
                }
            }
        } else {
            std::cerr << "Directory not found: " << folderPath << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error emptying folder: " << e.what() << std::endl;
        return false;
    }

    return true;
}
