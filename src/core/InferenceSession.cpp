#include "InferenceSession.hpp"
#include <iostream>
#include <codecvt>
#include <locale>
#include <algorithm> // For std::search

// DirectML provider header - usually available if onnxruntime is built with it
// #include <dml_provider_factory.h> 

namespace Core {

    InferenceSession::InferenceSession() 
        : env_(ORT_LOGGING_LEVEL_WARNING, "OfflinePhotoEnhancer") {
    }

    InferenceSession::~InferenceSession() {
    }

    bool InferenceSession::LoadModel(const std::wstring& modelPath, Device device) {
        // MOCK/STUB MODE for testing without real models
        if (modelPath.find(L"stub") != std::wstring::npos) {
            std::cout << "[Mock] Loading stub model..." << std::endl;
            // Set up dummy node names so Run() doesn't crash
            inputNodeNameAllocations_.push_back("input");
            inputNodeNames_.push_back(inputNodeNameAllocations_.back().c_str());
            
            outputNodeNameAllocations_.push_back("output");
            outputNodeNames_.push_back(outputNodeNameAllocations_.back().c_str());
            return true;
        }

        sessionOptions_ = Ort::SessionOptions();
        sessionOptions_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        if (device == Device::DirectML) {
            try {
                OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions_, 0);
            } catch (const std::exception& e) {
                std::cerr << "Failed to enable DirectML: " << e.what() << ". Falling back to CPU." << std::endl;
            }
        } else if (device == Device::CUDA) {
            try {
                OrtCUDAProviderOptions cuda_options;
                sessionOptions_.AppendExecutionProvider_CUDA(cuda_options);
            } catch (const std::exception& e) {
                std::cerr << "Failed to enable CUDA: " << e.what() << ". Falling back to CPU." << std::endl;
            }
        }

        try {
            session_ = std::make_unique<Ort::Session>(env_, modelPath.c_str(), sessionOptions_);
        } catch (const Ort::Exception& e) {
            std::cerr << "Failed to load model: " << e.what() << std::endl;
            return false;
        }

        // Resolve Input/Output names
        Ort::AllocatorWithDefaultOptions allocator;
        
        size_t numInputNodes = session_->GetInputCount();
        inputNodeNames_.clear();
        inputNodeNameAllocations_.clear();
        for (size_t i = 0; i < numInputNodes; i++) {
            auto name = session_->GetInputNameAllocated(i, allocator);
            inputNodeNameAllocations_.push_back(name.get());
            inputNodeNames_.push_back(inputNodeNameAllocations_.back().c_str());
        }

        size_t numOutputNodes = session_->GetOutputCount();
        outputNodeNames_.clear();
        outputNodeNameAllocations_.clear();
        for (size_t i = 0; i < numOutputNodes; i++) {
            auto name = session_->GetOutputNameAllocated(i, allocator);
            outputNodeNameAllocations_.push_back(name.get());
            outputNodeNames_.push_back(outputNodeNameAllocations_.back().c_str());
        }

        return true;
    }

    std::vector<float> InferenceSession::Run(const std::vector<float>& inputData, const std::vector<int64_t>& inputDims) {
        // MOCK MODE
        if (!session_) {
            // Assume stub mode if session_ is null but we are inside Run (LoadModel returned true for stub)
            // We just return the input data scaled up? Or just same size?
            // Engine expects output to be scale * input.
            // Let's assume the caller expects 4x scale for the stub.
            
            // For simplicity, let's just return the input data repeated 16 times (4x4) or just resized?
            // Actually, the Engine logic expects specific output size.
            // Let's just return a vector of size input_size * 16 (for 4x scale).
            // This is just noise, but it proves the pipeline flows.
            size_t inputSize = inputData.size();
            std::vector<float> dummyOutput(inputSize * 16, 0.5f); // Grey image
            return dummyOutput;
        }

        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        
        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo, 
            const_cast<float*>(inputData.data()), 
            inputData.size(), 
            inputDims.data(), 
            inputDims.size()
        );

        try {
            auto outputTensors = session_->Run(
                Ort::RunOptions{nullptr}, 
                inputNodeNames_.data(), 
                &inputTensor, 
                1, 
                outputNodeNames_.data(), 
                outputNodeNames_.size()
            );

            if (outputTensors.empty()) return {};

            float* floatarr = outputTensors[0].GetTensorMutableData<float>();
            size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
            
            return std::vector<float>(floatarr, floatarr + count);

        } catch (const Ort::Exception& e) {
            std::cerr << "Inference failed: " << e.what() << std::endl;
            return {};
        }
    }

    std::vector<int64_t> InferenceSession::GetInputShape() const {
        if (!session_) return {1, 3, 256, 256}; // Stub shape
        if (session_->GetInputCount() == 0) return {};
        return session_->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
    }

}
