#pragma once

#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace Core {

    enum class Device {
        CPU,
        CUDA,
        DirectML
    };

    class InferenceSession {
    public:
        InferenceSession();
        ~InferenceSession();

        // Load model from path.
        bool LoadModel(const std::wstring& modelPath, Device device = Device::CPU);

        // Run inference.
        // inputData: CHW float vector.
        // inputDims: {batch, channels, height, width}
        // Returns output data as flat vector.
        std::vector<float> Run(const std::vector<float>& inputData, const std::vector<int64_t>& inputDims);

        // Get expected input shape (if static)
        std::vector<int64_t> GetInputShape() const;

    private:
        Ort::Env env_;
        std::unique_ptr<Ort::Session> session_;
        Ort::SessionOptions sessionOptions_;
        
        std::vector<const char*> inputNodeNames_;
        std::vector<const char*> outputNodeNames_;
        
        // We need to keep the strings alive for the char* pointers
        std::vector<std::string> inputNodeNameAllocations_;
        std::vector<std::string> outputNodeNameAllocations_;
    };

}
