/**
 * @file ModelProfile.h
 * @brief Header file for ModelProfile class for logging model statistics
 */

#ifndef MODEL_STATS_H
#define MODEL_STATS_H

#include <string>
#include <vector>
#include <map>
#include "saver.h"
#include "generic.h"
#include <kll_sketch.hpp>
#include <frequent_items_sketch.hpp>

// Assuming declarations for Saver, distributionBox, ClassificationResult, and YoloDetection

/**
 * @brief Class for managing and logging model statistics
 */
typedef datasketches::kll_sketch<float> distributionBox;
typedef std::vector<std::pair<float, int>> ClassificationResults;
typedef datasketches::frequent_items_sketch<std::string> frequent_class_sketch;

class ModelProfile {
public:
  /**
   * @brief Constructor to initialize ModelProfile object
   * @param model_id Unique identifier for the model
   * @param no_of_classes Number of classes the model predicts
   * @param saver Reference to a Saver object used for saving model statistics
   */
  ModelProfile(std::string model_id, std::string conf_path,
	       	int save_interval, int top_classes);
  ~ModelProfile();

  /**
   * @brief Logs statistics for a classification model
   * @param inference_latency Time taken for model inference
   * @param results Reference to the classification results
   * @return 0 on success, negative value on error
   */
  int log_classification_model_stats(float inference_latency, const ClassificationResults& results);
  /**
   * @brief Logs model embeddings from the model
   * @param vector of embeddings
   * return 0 on sucess, negative value on error
   * */
  int log_embeddings(const std::vector<float>& embeddings);
  /**
   * @brief Logs Class wise model embeddings
   * @param vector of embeddings
   * @param class id
   * return 0 on success , negative value on error
   * */
  int log_embeddings(const std::vector<float>& embeddings, int cls); 

  frequent_class_sketch *sketch1;
  int getNumDistributionBoxes() const;
  const distributionBox& getDistributionBox(unsigned int index) const;
  void registerStatistics();
#ifndef TEST
  private:
#endif

  Saver *saver;
  // Member variables (declarations only, definitions in .cpp file)
  std::string model_id_;
  int top_classes_;
  std::string statSavepath;
  std::string dataSavepath;
  std::map<std::string, std::vector<std::string>> modelConfig; 
  std::vector<float> inference_latency_;
  std::vector<int> no_detections_per_image_;
  std::vector<double> objectnessbox_;
  distributionBox *dBox;
  std::map<int, distributionBox*> model_classes_stat_;
  distributionBox *model_embeddings;
  /**
  * @brief Gets the distributionBox (KLL sketch) for the given statistic name.
  * If it doesn't exist, a new one is created and added to the map.
  * @param name Statistic name
  * @return Pointer to the distributionBox object
  */
  distributionBox* getBox(const int cls);
  // Map to store KLL sketches based on statistic names
  std::unordered_map<int, distributionBox*> embeddings_stat_;
  
};

#endif // MODEL_STATS_H
