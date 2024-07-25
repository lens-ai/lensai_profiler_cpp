#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/string_util.h"
#include "tensorflow/lite/examples/label_image/get_top_n.h"
#include "tensorflow/lite/model.h"
#include <frequent_items_sketch.hpp>
#include <imagesampler.h>
#include <imageprofile.h>
#include <modelprofile.h>
#include <http_uploader.h>
#include <chrono>

namespace fs = std::__fs::filesystem;

std::vector<std::string> load_labels(const std::string& labels_file) {
    std::ifstream file(labels_file);
    if (!file.is_open()) {
        fprintf(stderr, "Unable to open label file\n");
        exit(-1);
    }
    std::string label_str;
    std::vector<std::string> labels;

    while (std::getline(file, label_str)) {
        if (!label_str.empty())
            labels.push_back(label_str);
    }
    file.close();
    return labels;
}

void printTensorInfo(tflite::Interpreter* interpreter, int tensor_index) {
    auto tensor = interpreter->tensor(tensor_index);
    if (tensor) {
        std::cout << "Tensor index: " << tensor_index << std::endl;
        std::cout << "Tensor name: " << tensor->name << std::endl;
        std::cout << "Tensor type: " << tensor->type << std::endl;
        std::cout << "Tensor dimensions: ";
        for (int i = 0; i < tensor->dims->size; ++i) {
            std::cout << tensor->dims->data[i] << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Failed to get tensor info for tensor index " << tensor_index << std::endl;
    }
}

void run_inference_on_image(const std::string& imageFile, tflite::Interpreter* interpreter, const std::vector<std::string>& labels, const std::string& model_name, ImageProfile &image_profile, 
	       ModelProfile &model_profile, ImageSampler &image_sampler) {
    // Get Input Tensor Dimensions
    int input = interpreter->inputs()[0];
    auto height = interpreter->tensor(input)->dims->data[1];
    auto width = interpreter->tensor(input)->dims->data[2];
    auto channels = interpreter->tensor(input)->dims->data[3];
    
    // Load Input Image
    cv::Mat image;
    auto frame = cv::imread(imageFile);
    if (frame.empty()) {
        fprintf(stderr, "Failed to load image: %s\n", imageFile.c_str());
        return;
    }
    TfLiteTensor* input_tensor = interpreter->input_tensor(0);
    int tensor_index = interpreter->inputs()[0];
    auto tensor = interpreter->tensor(tensor_index);
    if (tensor) {
        std::cout << "Tensor index: " << tensor_index << std::endl;
        std::cout << "Tensor name: " << tensor->name << std::endl;
        std::cout << "Tensor type: " << tensor->type << std::endl;
        std::cout << "Tensor dimensions: ";
        for (int i = 0; i < tensor->dims->size; ++i) {
            std::cout << tensor->dims->data[i] << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Failed to get tensor info for tensor index " << tensor_index << std::endl;
    }

    // Copy image to input tensor
    cv::resize(frame, image, cv::Size(width, height), cv::INTER_NEAREST);
    size_t tensor_size = input_tensor->bytes;
    size_t image_size = image.total() * image.elemSize();
    
    // Convert the image to float (32-bit floating point)
    image.convertTo(image, CV_32FC3);
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

    // Normalize the image to [0, 1] if needed (comment this out if your model expects values in a different range)
    // image = image / 255.0;

    // Ensure the type is correct
    if (image.type() != CV_32FC3) {
        std::cerr << "Error: Resized image is not of type CV_32FC3" << std::endl;
        return ;
    } 
    size_t expected_size = width * height * channels * sizeof(float);
        if (expected_size != input_tensor->bytes) {
            std::cerr << "Size mismatch: Resized image size (" << image_size << ") does not match input tensor size (" << input_tensor->bytes << ")" << std::endl;
            return;
        }
    std::cout << image.type() << std::endl;
      int depth = image.depth();

    switch (depth) {
      case CV_8U:
          std::cout << "Image data type: CV_8U (unsigned char)" << std::endl;
          break;
      case CV_16U:
         std::cout << "Image data type: CV_16U (unsigned short)" << std::endl;
         break;
      case CV_32F:
         std::cout << "Image data type: CV_32F (float)" << std::endl;
         break;
      case CV_64F:
         std::cout << "Image data type: CV_64F (double)" << std::endl;
         break;
      default:
      std::cout << "Unknown image data type!" << std::endl;
    }    
  
    memcpy(interpreter->typed_input_tensor<float>(0), image.data, image.total() * image.elemSize());
      // Inference
    std::chrono::steady_clock::time_point start, end;
    start = std::chrono::steady_clock::now();
    interpreter->Invoke();
    end = std::chrono::steady_clock::now();
    
    auto inference_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Get Output
    int output = interpreter->outputs()[0];
    const TfLiteTensor* outputTensor = interpreter->tensor(output);
    int output_size = outputTensor->dims->data[1]; // Assuming the shape is [1, 2]

    // Ensure the output tensor has the expected size
    if (output_size != 1) {
        std::cerr << "Unexpected output tensor size: " << output_size << std::endl;
        return;
    }

    // Retrieve the output tensor data
    float* output_data = interpreter->typed_output_tensor<float>(0);

    float threshold = 0.5f;
    // Calculate the probabilities
    float class_0_prob = output_data[0];
    float class_1_prob = 1.0f - class_0_prob;
    std::vector<std::pair<float, int>> top_results;
    // Populate the results vector
    if (class_0_prob > threshold) {
        top_results.push_back(std::make_pair(class_0_prob, 0)); // Class 0
        top_results.push_back(std::make_pair(class_1_prob, 1)); // Class 1
    } else {
        top_results.push_back(std::make_pair(class_1_prob, 1)); // Class 1
        top_results.push_back(std::make_pair(class_0_prob, 0)); // Class 0
    }

    cv::putText(frame, "Inference Time: " + std::to_string(inference_time) + " ms", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);

    // Print labels with confidence on the image
    for (const auto& result : top_results) {
        const float confidence = result.first;
        const int index = result.second;
        std::string output_txt = "Label: " + labels[index] + " Confidence: " + std::to_string(confidence);
	std::cout << output_txt << std::endl;
        //cv::putText(frame, output_txt, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
    }

    std::cout << "profiling image profile" <<std::endl;
    start = std::chrono::high_resolution_clock::now();
    image_profile.profile(frame, true);
    end = std::chrono::high_resolution_clock::now();
    auto execution_time_imageprofile = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time for image profile: " << execution_time_imageprofile <<std::endl; 
    
    std::cout << "profiling model profile" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    model_profile.log_classification_model_stats(10.0, top_results);
    end = std::chrono::high_resolution_clock::now();
    auto execution_time_modelprofile = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time for model profile: " << execution_time_modelprofile << std::endl;

    std::cout << "profiling samper" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    image_sampler.sample(top_results, image, true);
    end = std::chrono::high_resolution_clock::now();
    auto execution_time_sampler = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time for image sampler: " << execution_time_sampler << std::endl;
    std::cout << "Execution time inference :" << inference_time << std::endl;
    // Display image
    //cv::imshow("Output", frame);
    //cv::waitKey(0);
}

int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <modelfile> <labelfile> <image_folder> <configfile>\n", argv[0]);
        return -1;
    }
    const char* modelFileName = argv[1];
    const char* labelFile = argv[2];
    const char* imageFolder = argv[3];
    const char* configFile = argv[4];

    // Load Model
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile(modelFileName);
    if (!model) {
        fprintf(stderr, "Failed to load model\n");
        return -1;
    }

    // Initiate Interpreter
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (!interpreter) {
        fprintf(stderr, "Failed to initiate the interpreter\n");
        return -1;
    }

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        fprintf(stderr, "Failed to allocate tensors\n");
        return -1;
    }

    // Configure the interpreter
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(1);

    // Load Labels
    auto labels = load_labels(labelFile);
   

    int saveIntervalSec = 1;
    int channels = 3;
    std::string modelName = "Mobilenet_Binary";

    ImageProfile image_profile(configFile, saveIntervalSec, channels);
    ModelProfile model_profile(modelName, configFile, saveIntervalSec, channels);
    ImageSampler image_sampler(configFile, saveIntervalSec);
    HttpUploader http_uploader(configFile);
    http_uploader.StartUpload();
    // Traverse image folder and run inference on each image
    for (const auto& entry : fs::directory_iterator(imageFolder)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".JPEG" || entry.path().extension() == ".png")) {
            std::cout << "Processing image: " << entry.path().string() << std::endl;
            run_inference_on_image(entry.path().string(), interpreter.get(), labels, modelFileName,
			    image_profile, model_profile, image_sampler);
        }
    }

    return 0;
}

