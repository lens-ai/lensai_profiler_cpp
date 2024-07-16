#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include <string>
#include <ctime>

class HttpUploader {
public:
    HttpUploader(const std::string& endpointUrl, const std::string& token);

    // Function to upload a file with sensor ID, timestamp, and file type
    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType);

private:
    std::string endpointUrl_;
    std::string token_;
};

#endif // HTTP_UPLOADER_H
