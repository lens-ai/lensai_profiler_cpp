#include "libs/sketches.h"

/**
 * @brief A sketch data structure for approximate quantile estimation using the KLL algorithm.
 *
 * This class implements the KLL sketch data structure, which can be used to estimate quantiles of a data stream in an approximate but memory-efficient way.
 */
  /**
   * @brief Constructs a KLLSketch object with the specified maximum number of bins.
   *
   * @param maxBins The maximum number of bins to store in the sketch.
   */
  KLLSketch::KLLSketch(int maxBins) : maxBins(maxBins) {
    bins.reserve(maxBins);
  }

  /**
   * @brief Updates the sketch with a new data point.
   *
   * This function adds a new data point to the sketch and maintains the internal state for quantile estimation. If the number of data points exceeds the maximum number of bins, the mergeBins() function is called to reduce the number of bins.
   *
   * @param value The data point to be added to the sketch.
   */
  void KLLSketch::update(double value) {
    bins.push_back(value);
    std::sort(bins.begin(), bins.end());
    if (bins.size() > maxBins) {
      // Merge bins if we have more than maxBins
      mergeBins();
    }
  }

  /**
   * @brief Estimates the quantile of the data stream represented by the sketch.
   *
   * This function estimates the value at a specific quantile (e.g., median) based on the data points stored in the sketch.
   *
   * @param quantile The quantile value (between 0.0 and 1.0).
   * @return The estimated value at the specified quantile.
   */
  double KLLSketch::getQuantile(double quantile) {
    if (bins.empty()) {
      return 0.0; // or handle this case as you need
    }
    int rank = static_cast<int>(quantile * bins.size());
    return bins[rank];
  }

  /**
   * @brief Merges bins to maintain the maximum number of bins in the sketch.
   *
   * This function reduces the number of bins in the sketch to the specified maximum by averaging the values within equally sized bins.
   */
  void KLLSketch::mergeBins() {
    std::vector<double> newBins;
    int binSize = bins.size() / maxBins;
    for (int i = 0; i < bins.size(); i += binSize) {
      double binSum = 0;
      for (int j = i; j < i + binSize; ++j) {
        binSum += bins[j];
      }
      newBins.push_back(binSum / binSize);
    }
    bins = newBins;
  }
