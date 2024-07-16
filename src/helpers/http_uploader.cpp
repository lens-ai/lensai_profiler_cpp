#include "http_uploader.h"
#include <fstream>
#include <vector>
#include <ctime>
#include <curl/curl.h>
#include <filesystem> // For file deletion

HttpUploader::HttpUploader(const std::string& endpointUrl, const std::string& token)
    : endpointUrl_(endpointUrl), token_(token) {}

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

    if (res == CURLE_OK) {
        try {
            std::filesystem::remove(filePath);
            std::cout << "Successfully deleted file: " << filePath << std::endl;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error deleting file: " << e.what() << std::endl;
            return false;
        }
        return true;
    } else {
        std::cerr << "HTTP POST request failed: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
}

