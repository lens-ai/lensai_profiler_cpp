/**

@file http_uploader.cpp
@brief Implementation of the HttpUploader class for uploading files via HTTP.
*/

#include <fstream>
#include <vector>
#include <ctime>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <unistd.h>

#include "http_uploader.h"
#include "iniparser.h"
#include "generic.h"
#include "datatracer_log.h"

namespace fs = std::filesystem;

#define uploader_err log_err << uploader_name_ << ": "
#define uploader_info log_info << uploader_name_ << ": "
#if DEBUG
#define uploader_debug log_debug << uploader_name_ << ": "
#else
#define uploader_debug if (0) std::cout
#endif

/**

@brief Destructor for HttpUploader. Ensures upload process is stopped.
*/

HttpUploader::~HttpUploader() {
    {
        std::unique_lock<std::mutex> lock(upload_mutex_);
        StopUpload();
    } 
}

/**

@brief Constructor for HttpUploader.

@param conf_path Path to the configuration file.

@param uploader_name Name of the uploader instance.
*/

HttpUploader::HttpUploader(const std::string& conf_path, const std::string& uploader_name)
:uploader_name_(uploader_name) {
    try { 
        // Read configuration settings                                        
        IniParser parser;
        lensaipublisherConfig = parser.parseIniFile(conf_path, uploader_name, "");

        http_uploader_data_.endpointUrl = lensaipublisherConfig["http_endpoint"][0];
        http_uploader_data_.token = lensaipublisherConfig["token"][0];
        http_uploader_data_.sensorId = lensaipublisherConfig["sensorId"][0];
        uploader_info << lensaipublisherConfig["folderPath"].size() << std::endl;

        for (auto i=0; i < lensaipublisherConfig["folderPath"].size(); i++) {
            uploader_info << "[" << i << "]" << std::endl;
            uploader_info << "\tfolderPath" << "=" << lensaipublisherConfig["folderPath"][i] << std::endl;
            http_uploader_data_.folderPath.push_back(lensaipublisherConfig["folderPath"][i]);

            uploader_info << "\tfileType" << "=" << lensaipublisherConfig["fileType"][i] << std::endl;
            http_uploader_data_.fileType.push_back(lensaipublisherConfig["fileType"][i]);

            uploader_info << "\tdeletedata" << "=" << lensaipublisherConfig["deletedata"][i] << std::endl;
            bool deletedata =  (lensaipublisherConfig["deletedata"][i] == "true");
            http_uploader_data_.deletedata.push_back(deletedata);
        }

        std::string upload_interval_str = lensaipublisherConfig["upload_interval"][0];
        http_uploader_data_.interval = atoi(upload_interval_str.c_str());
        

    } catch (const std::runtime_error& e) {
        uploader_err << e.what() << std::endl;
    }
}

/**

@brief Starts the upload process in a separate thread.
*/

void HttpUploader::StartUpload() {
    exitUploadLoop.store(false);
    upload_thread_ = std::thread(&HttpUploader::UploadLoop, this);
}

/**

@brief The main loop for uploading files. This function runs in a separate thread.
*/

void HttpUploader::UploadLoop() {
    int index = 0;
    while (true) {
        do {

            if (exitUploadLoop.load()) {
              pthread_exit(nullptr);// Thread termination condition
            }

            std::unique_lock<std::mutex> lock(upload_mutex_);

            if (exitUploadLoop.load()) {
                pthread_exit(nullptr);// Thread termination condition
            }

            uploadFolder(index);
            index++;
            if (index >= http_uploader_data_.folderPath.size())
                index = 0;
        }while(0); //scope of queue_mutex_

        for (int i = 0; i < http_uploader_data_.interval; i++) {
            if (exitUploadLoop.load()) {
                pthread_exit(nullptr);// Thread termination condition
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

/**

@brief Stops the upload process by joining the upload thread.
*/


void HttpUploader::StopUpload(void) {
    if (upload_thread_.joinable()) {
        exitUploadLoop.store(true);
        upload_thread_.join();
    }
}

/**

@brief Uploads a folder by first creating a tar.gz archive and then sending it via HTTP.

@param index Index of the folder in the configuration.

@return True if the upload was successful, false otherwise.
*/

bool HttpUploader::uploadFolder(int &index) {
    bool ret = false;
    // Get current time as time_point
    auto now = std::chrono::system_clock::now();
    // Convert to time_t
    std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
    TarGzCreator tarGzCreator;

    // Step 1: Create tar file
    std::vector<std::string> folders = { http_uploader_data_.folderPath[index]};
    std::vector<std::string> files = tarGzCreator.collectFilesFromFolders(folders);
    std::string tarFilePath = http_uploader_data_.folderPath[index] + "_archive_lock.tar";
    std::string gzFilePath = http_uploader_data_.folderPath[index] + "_archive_lock.tar.gz";

    int fd = acquire_lock(folders[0]);
    if (fd == -1) {
        uploader_err << "Failed to acquire_lock." << std::endl;
        return ret;
    }

    if (!tarGzCreator.createTar(tarFilePath, files, http_uploader_data_.folderPath[index])) {
        uploader_err << "Failed to create tar file." << std::endl;
        goto del_tar_gz;
    }

    // Step 2: Compress to tar.gz
    if (!tarGzCreator.compressToGz(tarFilePath, gzFilePath)) {
        uploader_err << "Failed to compress tar file to gz." << std::endl;
        goto del_tar_gz;
    }

    // Step 3: Upload the file
    for(int retry = 0; retry < UPLOAD_RETRY_COUNT; retry++) {
        if (postFile(gzFilePath, http_uploader_data_.sensorId, timestamp, http_uploader_data_.fileType[index])) {
            ret = true;
            // Step 4: Empty the folder
            uploader_info << "emptyFolder " << http_uploader_data_.folderPath[index] << " : " << http_uploader_data_.deletedata[index] << std::endl;
            if (!tarGzCreator.emptyFolder(http_uploader_data_.folderPath[index]) && http_uploader_data_.deletedata[index]) {
                uploader_err << "Failed to empty the folder." << std::endl;
            }
            goto del_tar_gz;
        }
        uploader_err << "Failed to upload gz file. Try - " << retry << std::endl;
        sleep(1);
    }

del_tar_gz:
    release_lock(fd);

    // Step 5: Delete the gz file
    if (fs::exists(gzFilePath)) {
        fs::remove(gzFilePath);
    }

    // Step 6: Delete the tar file
    if (fs::exists(tarFilePath)) {
        fs::remove(tarFilePath);
    }

    return ret;
}

/**

@brief Sends the specified file via HTTP POST.

@param filePath Path to the file to upload.

@param sensorId Sensor ID to include in the upload metadata.

@param timestamp Timestamp of the upload.

@param fileType Type of the file being uploaded.

@return True if the file was successfully uploaded, false otherwise.
*/

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

    curl_easy_setopt(curl, CURLOPT_URL, http_uploader_data_.endpointUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + http_uploader_data_.token).c_str());
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

