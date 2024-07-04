/**
 * @file ImageUploader.h
 * @brief Header file for the ImageUploader class used for uploading images to AWS S3
 */

#ifndef IMAGE_UPLOADER_H
#define IMAGE_UPLOADER_H

#ifdef AWS
#include <aws/core/utils/threading/Executor.h>
#include <aws/transfer/TransferManager.h>
#include <aws/core/auth/AWSCredentials.h>
#endif

#include <chrono>
#include <mutex>
#include <atomic>

#include "http_uploader.h"

typedef struct {
#ifdef AWS
	Aws::Auth::AWSCredentials credentials;  ///< AWS credentials for S3 access
	Aws::String region;                    ///< AWS region where the S3 bucket resides
#else
	int dummy;
#endif	
}s3_client_config_t;

/**
 * @class ImageUploader
 * @brief Class for uploading images to AWS S3 in a separate thread
 */
class ImageUploader {
public:
  /**
   * @brief Constructor to initialize the ImageUploader object
   * @param credentials AWS credentials for S3 access
   * @param region AWS region where the S3 bucket resides
   * @param executor Optional thread executor for upload tasks (defaults to PooledThreadExecutor)
   */
  ImageUploader(int uploadtype, const std::string& endpointUrl, const std::string& token,
                s3_client_config_t &s3_client_config);
  /**
   * @brief Starts a thread that uploads the image to S3 in a loop at a specified interval
   * @param imagePath Path to the image file on local storage
   * @param bucketName Name of the S3 bucket where the image will be uploaded
   * @param objectKey Name of the object (filename) in the S3 bucket
   * @param interval Upload interval in milliseconds
   * @return true on success, false on error
   */
  bool startUploadThread(const std::string& imagePath, const std::string& bucketName,
                         const std::string& objectKey, const std::chrono::milliseconds& interval);

  /**
   * @brief Stops the running upload thread
   */
  void stopUploadThread();

private:
  int type;
    s3_client_config_t s3_client_config_;
  std::atomic<bool> stopFlag_;             ///< Flag to indicate stopping the upload thread
  std::mutex uploadMutex_;                 ///< Mutex for thread-safe image upload
    HttpUploader * _HttpUploader;
  // Function to run the upload thread (implementation in ImageUploader.cpp)
  void uploadThread(const std::string& imagePath, const std::string& bucketName,
                    const std::string& objectKey, const std::chrono::milliseconds& interval);
};

#endif // IMAGE_UPLOADER_H

