![Build Status](https://github.com/lens-ai/lensai_profiler_cpp/actions/workflows/ci.yaml/badge.svg)

# LensAI Profiler Cpp

Lensai profiler Cpp is a C++ library designed for AI model observability and  data sampling in real-time, specifically focused on edge devices.
The goal of lens AI is to Manage, debug, and retrain AI models at the edge efficiently.
Lens AI Cpp profiler includes three libraries: `libimageprofiler.so`,  `libimagesampler.so`, and `libmodelprofiler.so`.
The current version of LensAI is focussed towards Vision but soon will support the other data types.

![Block Diagram](https://raw.githubusercontent.com/lens-ai/lensai_profiler_cpp/main/blockdiagram_cpp.png)

## Model monitoring pipeline for Edge devices

### Model Training - Use the lensai_profiler_python 
During the model training process, It is important to profile the base line metrics, so that later they can be used for the drift and error approximation.
use the lensai_profiler_python : [github](https://github.com/lens-ai/lensai_profiler_python) library to profile the training data. This step is crucial for establishing baseline profiles for data and model performance.

### Model Deployment - Use the lensai_profiler_cpp (As most of the edge models inference happens using C++)
During model deployment, integrate this Lensai C++ profiling code. This integration profiles athe real-time data and model metrics during inference, enabling continuous monitoring. It also samples the data that the model is most uncertain about using multiple sampling techniques.

### Monitoring - Use lensai Monitoring Server 
Use the LensAI Server to monitor data and model drift by comparing profiles from the training phase and real-time data obtained during inference. [github](https://github.com/lens-ai/lensai_monitoring_server) Detailed instructions and implementation examples are available in the examples directory.

---

## Key Advantages of using Lens AI summarized
- The time complexity of the distribution profiles are sublinear space and time
- The space complexity is  𝑂(1/𝜖log(𝜖𝑁)) where as classical logging it is O(N).
- The time complexity while insertion and query time is O(log(1/ϵ)), where as classical logging it is O(N log(N))

---

## Memory Efficiency
#### Fixed Memory Usage:
The data structures used by Lens AI operate with a fixed amount of memory, making them ideal for applications with memory constraints or long-running processes where memory usage must remain predictable and bounded.

#### Scalability:
Unlike classical histograms that might require more memory as more data is processed or as data complexity increases (e.g., higher resolution, multiple channels), these sketches maintain a consistent memory footprint, ensuring efficient scalability.

---

## Accuracy and Error Bounds
#### Provable Error Guarantees:
Lens AI's data structures provide quantifiable error bounds for their approximations. This allows control over the trade-off between memory usage and accuracy, ensuring reliable results within acceptable error margins.

#### Adaptability:
These data structures dynamically adjust to the data distribution, providing better accuracy for varying data, unlike classical histograms that might become less accurate if the data distribution changes significantly.

---

## Flexibility in Queries
#### Quantile Queries:
Lens AI leverages KLL sketches for highly efficient quantile queries, essential for understanding data distributions and for various statistical analyses. Classical histograms require additional processing to derive quantiles, which can be computationally expensive and less accurate.

#### Versatile Statistical Summaries:
The datastructures allow for efficient computation of a wide range of statistical summaries, making them suitable for complex data analysis tasks beyond simple frequency counts.

---

## Computational Efficiency
#### Stream Processing:
 KLL datastructures are designed for incremental updates, making them highly efficient for streaming data scenarios. Each insertion has an amortized logarithmic time complexity, which is manageable even for high-throughput data streams.

#### Avoiding Reprocessing:
In streaming data applications, the sketches handle data incrementally without the need to reprocess entire datasets, unlike classical methods that might require recalculating histograms from scratch when new data arrives.

---

## Practical Advantages
#### Consistent Performance:
In practice, these sketches offer high accuracy for quantile estimation with significantly less memory usage than exact methods, making them a practical choice for many real-world applications.

#### Handling High-Dimensional Data:
In scenarios involving high-resolution images or multiple channels (e.g., RGB), the sketches manage the complexity efficiently, avoiding the pitfalls of multidimensional histograms which can be cumbersome and memory-intensive.

---

## Simplified Data Management
#### Automatic Range Adjustment:
The sketches automatically adjust to the range and distribution of incoming data, reducing the need for manual tuning of bin ranges and resolutions, often required in classical histograms.

#### Unified Data Structure:
Using a single, unified data structure for summarizing distributions simplifies data management and processing pipelines, reducing complexity and potential for errors.

---

## Mergeability Across Sensors:
The sketches are designed for distributed computing environments where data may be processed across multiple nodes or systems. They can be merged efficiently across nodes to provide a unified view of the data distribution, enabling scalable processing of large datasets.

---

## Product Dashboard:
Please find the product roadmap and next planned releases [here]()

---

## Prerequisites
Before you begin, ensure you have met the following requirements:
- libboost-all-dev
- libcurl4-openssl-dev
- libtar-dev
- libssl-dev
- libgtest-dev
- libopencv-dev

## Table of Contents

- [Installation](#installation)
  - [From Source](#from-source)
  - [Prebuilt Packages](#prebuilt-packages)
  - [Docker Images](#docker-images)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation
#### Prerequisites
- C++17 or higher
- CMake 3.10 or higher

#### From Source
##### Clone the repository
   ```sh
   https://github.com/lens-ai/lensai_profiler_cpp.git
   cd datatracer
  mkdir build
  ```
##### Build libraries with debug
   ```sh
   cd build
   cmake ..
   make
  ```
##### Build for Release
  ```sh
  cd build
  cmake .. -D CMAKE_BUILD_TYPE=RELEASE
  make
  ```
##### Run test
  ```sh
  cd build
  cmake ..
  make
  make test
  ```
##### Build Example
  ```sh
  cd build
  cmake ..
  make
  make TFLiteCheck
 ```
 ##### Build using docker
  ```sh
  cd Docker
  ./build.sh
  ```
#### Prebuilt Packages 
You can download and install prebuilt packages for various platforms:

Debian: Download Lensai-v1.0.0-linux.tar.gz
Raspian: Download Lensai-v1.0.0-linux.tar.gz

Installation Instructions

For Linux:
tar -xzf Lensai-v1.0.0-linux.tar.gz
sudo cp -r Lensai /usr/local/

#### Docker Images
- docker pull vsnm/lensai_profiler_cpp:latest
- docker run -it --rm vsnm/lensai_profiler_cpp

### Usage
Please refer the examples directory for the detailed implementation examples.

```cpp
  int saveIntervalSec = 1;
  int img_channels = 3;
  std::string modelName = "Cat_dog_classfier_0.1";

  ImageProfile image_profile(configFile, saveIntervalSec, channels);
  ModelProfile model_profile(modelName, configFile, saveIntervalSec, channels);
  ImageSampler image_sampler(configFile, saveIntervalSec);
  
  std::cout << "profiling image profile" <<std::endl;
  image_profile.profile(frame, true);
 
  std::cout << "profiling model profile" << std::endl;
  model_profile.log_classification_model_stats(10.0, top_results);
    
  std::cout << "profiling samper" << std::endl;
  image_sampler.sample(top_results, image, true);
  ```
Refer to the examples in the example folder

## API Reference:
Refer to the [documentation](github.com)
## Contributors
#### Venkata Pydipalli 
Looking for the more contributors with Love.
## License
Apache 2
## Credits
Apache Datasketches
## Contact
vsnm.tej@gmail.com
