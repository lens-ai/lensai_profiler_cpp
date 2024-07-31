/**
 * @file http_uploader.h
 * @brief Defines the HttpUploader class for managing file uploads over HTTP.
 */

#ifndef HTTP_UPLOADER_H
#define HTTP_UPLOADER_H

#include "tar_gz_creator.h"
#include <string>
#include <vector>
#include <ctime>
#include <thread>
#include <atomic>
#include <map>

/// Number of retry attempts for uploading a file.
#define UPLOAD_RETRY_COUNT 2

/**
 * @brief Struct to hold HTTP uploader configuration data.
 */
typedef struct {
    std::string endpointUrl;           ///< URL of the HTTP endpoint.
    std::string token;                 ///< Authorization token.
    std::vector<std::string> folderPath; ///< Paths of the folders to upload.
    std::string sensorId;              ///< Sensor ID to be used in the upload.
    std::vector<std::string> fileType; ///< File types to upload.
    std::vector<bool> deletedata;      ///< Flags to indicate whether to delete data after upload.
    int interval;                      ///< Interval in seconds between uploads.
} http_uploader_data_t;

/**
 * @brief The HttpUploader class handles uploading files to an HTTP server.
 */
class HttpUploader {
public:
    /**
     * @brief Destructor for HttpUploader.
     */
    ~HttpUploader();

    /**
     * @brief Constructor for HttpUploader.
     * @param conf_path Path to the configuration file.
     * @param uploader_name Name of the uploader instance.
     */
    HttpUploader(const std::string& conf_path, const std::string& uploader_name);

    /**
     * @brief Starts the upload process in a separate thread.
     */
    void StartUpload();

    /**
     * @brief Stops the upload process by joining the upload thread.
     */
    void StopUpload();

    /**
     * @brief The main loop for uploading files, executed in a separate thread.
     */
    void UploadLoop();

private:
    std::map<std::string, std::vector<std::string>> lensaipublisherConfig; ///< Configuration data from the INI file.
    http_uploader_data_t http_uploader_data_; ///< Data structure holding the uploader's configuration.

    std::thread upload_thread_;        ///< Thread object for handling the upload process.
    std::mutex upload_mutex_;          ///< Mutex for synchronizing access to shared resources.

    std::atomic<bool> exitUploadLoop;  ///< Atomic flag to signal stopping the upload loop.

    std::string uploader_name_;        ///< Name of the uploader instance.

    /**
     * @brief Uploads a file to the HTTP server.
     * @param filePath Path to the file to upload.
     * @param sensorId Sensor ID to include in the upload metadata.
     * @param timestamp Timestamp of the upload.
     * @param fileType Type of the file being uploaded.
     * @return True if the file was successfully uploaded, false otherwise.
     */
    bool postFile(const std::string& filePath, const std::string& sensorId, time_t timestamp, const std::string& fileType);

    /**
     * @brief Uploads the contents of a folder, creating a tar.gz file and sending it.
     * @param index Index of the folder in the configuration.
     * @return True if the upload was successful, false otherwise.
     */
    bool uploadFolder(int &index);
};

#endif // HTTP_UPLOADER_H
