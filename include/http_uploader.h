#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include "tar_gz_creator.h"
#include <string>
#include <vector>
#include <ctime>
#include <thread>
#include <atomic>
#include <map>

typedef struct {
    std::string endpointUrl;
    std::string token;
    std::vector<std::string> folderPath;
    std::string sensorId;
    std::vector<std::string> fileType;
    bool deletedata;
    int interval;
}http_uploader_data_t;

class HttpUploader {
public:
    ~HttpUploader();
    HttpUploader(const std::string& conf_path);

    void StartUpload();
    void StopUpload();
    void UploadLoop();
private:
    std::map<std::string, std::vector<std::string>> lensaipublisherConfig;
    http_uploader_data_t http_uploader_data_;

    std::thread upload_thread_;        // Thread object for upload
    std::mutex upload_mutex_;         // Mutex for queue access

    std::atomic<bool> exitUploadLoop;

    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType);
    bool uploadFolder(int &index);
};

#endif // HTTP_UPLOADER_H
