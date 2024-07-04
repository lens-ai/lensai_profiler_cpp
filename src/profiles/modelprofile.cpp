#include "modelprofile.h"
#include "iniparser.h"

ModelProfile::~ModelProfile() {
    delete saver;
    for (const auto& pair : model_classes_stat_) {
        delete pair.second;
    }
    delete sketch1;
}

/**
 * @class ModelProfile
 * @brief Class for computing and managing various model statistics
 */

/**
 * @brief ModelProfile class constructor
 * @param model_id Unique identifier for the model
 * @param no_of_classes Number of classes the model predicts
 * @param saver Reference to a Saver object used for saving model statistics
 */
ModelProfile::ModelProfile(std::string model_id, std::string conf_path,
	       	int save_interval, int top_classes) {
    std::string endpointUrl="";
    std::string token="";

  // Set member variables
  saver = new Saver(save_interval, "ModelProfile");
  model_id_ = model_id;
  IniParser parser;
  modelConfig = parser.parseIniFileNew(conf_path,
                          "model", "");
  filesSavePath = modelConfig["filepath"][0];
  createFolderIfNotExists(filesSavePath);
  top_classes_ = top_classes;
  sketch1 = new frequent_class_sketch(64);
  saver->StartSaving();
#ifndef TEST
    /*int uploadtype=0;
    s3_client_config_t s3_client_config; 
    std::string bucketName;
    std::string objectKey;
    //uploader = new ImageUploader(uploadtype, endpointUrl, token, s3_client_config);
    //uploader->startUploadThread(filesSavePath, bucketName, objectKey, interval);*/
#endif
}


// Public accessor to get the number of distribution boxes
int ModelProfile::getNumDistributionBoxes() const {
        return top_classes_;
    }

/**
 * @brief Logs classification model statistics
 * @param inference_latency Time taken for model inference
 * @param results Reference to the classification results
 * @return 0 on success, negative value on error
 *
 * This function iterates through the provided results and logs statistics for the most frequent classes.
 * It updates the `model_classes_stat` map with scores for each class.
 */
int ModelProfile::log_classification_model_stats(float inference_latency __attribute__((unused)),
	       	const ClassificationResults& results) {
    for (const auto& result : results) {
        int cls = result.second;
        float score = result.first;
        auto it = model_classes_stat_.find(cls);
        if (it != model_classes_stat_.end()) {
            // Key exists, update the value
            it->second->update(score);
        } else {
            // Key does not exist, add the key-value pair
            dBox = new distributionBox(200);
            model_classes_stat_[cls] = dBox;
            model_classes_stat_[cls]->update(score);
            saver->AddObjectToSave((void *)(dBox), KLL_TYPE,
                                   filesSavePath + model_id_ + std::to_string(cls) + ".bin");  // Register with Saver for saving
        }
        sketch1->update(std::to_string(cls));  // Placeholder for storing frequent class IDs
    }
  return 0; // Assuming successful logging, replace with error handling if needed
}
