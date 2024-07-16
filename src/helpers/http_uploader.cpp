#include "http_uploader.h"
#include <fstream>
#include <vector>
#include <ctime>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

HttpUploader::HttpUploader(const std::string& endpointUrl, const std::string& token)
    : endpointUrl_(endpointUrl), token_(token) {}

bool HttpUploader::uploadFolder(const std::string& folderPath, const std::string& sensorId, time_t timestamp, const std::string& fileType, bool deletedata) {
    TarGzCreator tarGzCreator;

    // Step 1: Create tar file
    std::vector<std::string> folders = { folderPath };
    std::vector<std::string> files = tarGzCreator.collectFilesFromFolders(folders);
    std::string tarFilePath = "archive.tar";
    std::string gzFilePath = "archive.tar.gz";

    if (!tarGzCreator.createTar(tarFilePath, files)) {
        std::cerr << "Failed to create tar file." << std::endl;
        return false;
    }

    // Step 2: Compress to tar.gz
    if (!tarGzCreator.compressToGz(tarFilePath, gzFilePath)) {
        std::cerr << "Failed to compress tar file to gz." << std::endl;
        return false;
    }

    // Step 3: Empty the folder
    if (!tarGzCreator.emptyFolder(folderPath) && deletedata) {
        std::cerr << "Failed to empty the folder." << std::endl;
        return false;
    }

    // Step 4: Upload the file
    if (!postFile(gzFilePath, sensorId, timestamp, fileType)) {
        std::cerr << "Failed to upload gz file." << std::endl;
        return false;
    }

    // Step 5: Delete the gz file
    if (fs::exists(gzFilePath)) {
        fs::remove(gzFilePath);
    }

    // Step 6: Delete the tar file
    if (fs::exists(tarFilePath)) {
        fs::remove(tarFilePath);
    }

    return true;
}

bool HttpUploader::postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    CURLcode res;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        curl_easy_cleanup(curl);
        return false;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> fileBuffer(fileSize);
    file.read(fileBuffer.data(), fileSize);

    curl_easy_setopt(curl, CURLOPT_URL, endpointUrl_.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token_).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    struct curl_httppost* formpost = nullptr;
    struct curl_httppost* lastptr = nullptr;

    // Adding metadata and the file to the form
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "sensor_id",
        CURLFORM_COPYCONTENTS, sensorId.c_str(),
        CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "timestamp",
        CURLFORM_COPYCONTENTS, std::to_string(timestamp).c_str(),
        CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "file_type",
        CURLFORM_COPYCONTENTS, fileType.c_str(),
        CURLFORM_END);

    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "file",
        CURLFORM_BUFFER, filePath.c_str(),
        CURLFORM_BUFFERLENGTH, fileBuffer.size(),
        CURLFORM_BUFFERPTR, fileBuffer.data(),
        CURLFORM_CONTENTTYPE, "application/gzip", // Specify the content type as gzip
        CURLFORM_END);

    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    res = curl_easy_perform(curl);

    curl_formfree(formpost);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}
