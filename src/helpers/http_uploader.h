#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include <string>
#include <curl/curl.h>

class HttpUploader {
public:
    HttpUploader(const std::string& endpointUrl, const std::string& token);
    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp);

private:
    std::string endpointUrl_;
    std::string token_;
};

#endif // HTTP_UPLOADER_H
