/**
 * @file KLLSketch.h
 * @brief Header file for the KLLSketch data structure.
 *
 * This header file defines the KLLSketch class, which implements a sketch data structure for approximate quantile estimation using the KLL algorithm.
 */

#ifndef KLL_SKETCH_H
#define KLL_SKETCH_H

#include <iostream>
#include <vector>
#include <algorithm>

/**
 * @class KLLSketch
 * @brief A sketch data structure for approximate quantile estimation.
 *
 * This class represents a KLL sketch, which can be used to estimate quantiles of a data stream in an approximate but memory-efficient way.
 */
class KLLSketch {
public:
  /**
   * @brief Constructs a KLLSketch object with the specified maximum number of bins.
   *
   * @param maxBins The maximum number of bins to store in the sketch.
   */
  KLLSketch(int maxBins);
  KLLSketch() {
	KLLSketch(10);
  };

  /**
   * @brief Updates the sketch with a new data point.
   *
   * This function adds a new data point to the sketch and maintains the internal state for quantile estimation. If the number of data points exceeds the maximum number of bins, the mergeBins() function is called to reduce the number of bins.
   *
   * @param value The data point to be added to the sketch.
   */
  void update(double value);

  /**
   * @brief Estimates the quantile of the data stream represented by the sketch.
   *
   * This function estimates the value at a specific quantile (e.g., median) based on the data points stored in the sketch.
   *
   * @param quantile The quantile value (between 0.0 and 1.0).
   * @return The estimated value at the specified quantile.
   */
  double getQuantile(double quantile);

private:
  /**
   * @brief Merges bins to maintain the maximum number of bins in the sketch.
   *
   * This function reduces the number of bins in the sketch to the specified maximum by averaging the values within equally sized bins.
   */
  void mergeBins();

  std::vector<double> bins; ///< The internal storage for data points in the sketch.
  int maxBins;              ///< The maximum number of bins allowed in the sketch.
};

#endif

