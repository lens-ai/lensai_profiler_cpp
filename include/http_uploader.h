#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include "tar_gz_creator.h"
#include <string>
#include <vector>
#include <ctime>

class HttpUploader {
public:
    HttpUploader(const std::string& endpointUrl, const std::string& token);
    bool uploadFolder(const std::string& folderPath, const std::string& sensorId, time_t timestamp, const std::string& fileType, bool deletedat = false);

private:
    std::string endpointUrl_;
    std::string token_;
    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType);
};

#endif // HTTP_UPLOADER_H
