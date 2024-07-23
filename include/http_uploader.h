#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include "tar_gz_creator.h"
#include <string>
#include <vector>
#include <ctime>
#include <thread>
#include <atomic>

typedef struct {
    std::string endpointUrl;
    std::string token;
    std::string folderPath;
    std::string sensorId;
    std::string fileType;
    bool deletedata;
    int interval;
}http_uploader_data_t;

class HttpUploader {
public:
    HttpUploader(http_uploader_data_t &http_uploader_data);

    void StartUpload();
    void StopUpload();
    void UploadLoop();
private:
    http_uploader_data_t http_uploader_data_;

    std::thread upload_thread_;        // Thread object for upload
    std::mutex upload_mutex_;         // Mutex for queue access

    std::atomic<bool> exitUploadLoop;

    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType);
    bool uploadFolder();
};

#endif // HTTP_UPLOADER_H
