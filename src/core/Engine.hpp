#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "InferenceSession.hpp"

namespace Core {

    struct EngineOptions {
        std::wstring modelPath;
        std::wstring faceModelPath; // Optional
        Device device = Device::CPU;
        int scale = 4;
        double strength = 0.5; // For sharpening/denoising mix
        bool enableFaceEnhance = false;
        int tileSize = 256; // Input tile size
        int tileOverlap = 16;
        bool keepExif = true;
    };

    struct ProgressEvent {
        std::string currentFile;
        int totalFiles;
        int currentFileIndex;
        float percentComplete; // 0.0 - 1.0
        std::string statusMessage;
    };

    using ProgressCallback = std::function<void(const ProgressEvent&)>;

    class Engine {
    public:
        Engine(const EngineOptions& options);
        ~Engine();

        // Initialize models (warmup)
        bool Initialize();

        // Process a single file
        bool ProcessFile(const std::wstring& inputPath, const std::wstring& outputPath);

        // Process a batch of files
        void ProcessBatch(const std::vector<std::wstring>& inputPaths, const std::wstring& outputDir, ProgressCallback callback);

    private:
        EngineOptions options_;
        std::unique_ptr<InferenceSession> session_;
        // std::unique_ptr<FaceEnhancer> faceEnhancer_; // TODO

        // Helper to process a single image in memory
        cv::Mat ProcessImage(const cv::Mat& input);
    };

}
