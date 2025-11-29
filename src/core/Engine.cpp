#include "Engine.hpp"
#include "ImageUtils.hpp"
#include <iostream>
#include <filesystem>
#include <cmath>

namespace Core {

    Engine::Engine(const EngineOptions& options) : options_(options) {
        session_ = std::make_unique<InferenceSession>();
    }

    Engine::~Engine() {}

    bool Engine::Initialize() {
        // Load main super-res model
        if (!std::filesystem::exists(options_.modelPath)) {
            std::wcerr << L"Model not found: " << options_.modelPath << std::endl;
            return false;
        }

        if (!session_->LoadModel(options_.modelPath, options_.device)) {
            std::cerr << "Failed to load model." << std::endl;
            return false;
        }

        // TODO: Load face model if enabled
        
        return true;
    }

    bool Engine::ProcessFile(const std::wstring& inputPath, const std::wstring& outputPath) {
        cv::Mat img = ImageUtils::LoadImage(inputPath);
        if (img.empty()) {
            std::wcerr << L"Failed to load image: " << inputPath << std::endl;
            return false;
        }

        cv::Mat result = ProcessImage(img);
        if (result.empty()) {
            return false;
        }

        // TODO: Handle EXIF copy if keepExif is true (requires external lib or specific OpenCV flags/manual copy)

        return ImageUtils::SaveImage(outputPath, result);
    }

    void Engine::ProcessBatch(const std::vector<std::wstring>& inputPaths, const std::wstring& outputDir, ProgressCallback callback) {
        int total = static_cast<int>(inputPaths.size());
        for (int i = 0; i < total; ++i) {
            std::filesystem::path p(inputPaths[i]);
            std::filesystem::path outDir(outputDir);
            
            // Construct output filename: name_upscaled.ext
            std::wstring stem = p.stem().wstring();
            std::wstring ext = p.extension().wstring();
            std::wstring outName = stem + L"_upscaled" + ext;
            std::filesystem::path outPath = outDir / outName;

            if (callback) {
                ProgressEvent evt;
                evt.currentFile = p.string();
                evt.totalFiles = total;
                evt.currentFileIndex = i + 1;
                evt.percentComplete = (float)i / total;
                evt.statusMessage = "Processing...";
                callback(evt);
            }

            ProcessFile(inputPaths[i], outPath.wstring());
        }
        
        if (callback) {
            ProgressEvent evt;
            evt.percentComplete = 1.0f;
            evt.statusMessage = "Done";
            callback(evt);
        }
    }

    cv::Mat Engine::ProcessImage(const cv::Mat& input) {
        // 1. Split into tiles
        // Note: We need to handle overlap and padding.
        // For simplicity in this skeleton, we assume the model output size = input size * scale.
        
        int scale = options_.scale;
        int tileSize = options_.tileSize;
        int overlap = options_.tileOverlap;

        // Pre-allocate canvas
        int outH = input.rows * scale;
        int outW = input.cols * scale;
        cv::Mat canvas = cv::Mat::zeros(outH, outW, CV_8UC3);

        std::vector<ImageTile> tiles = ImageUtils::SplitTiles(input, tileSize, overlap);
        
        // Get expected input shape from model to verify
        // std::vector<int64_t> inputShape = session_->GetInputShape(); 
        // Typically [1, 3, H, W] or dynamic.

        for (const auto& tile : tiles) {
            // Pre-process tile
            std::vector<float> inputData = ImageUtils::PreProcess(tile.data);
            std::vector<int64_t> inputDims = {1, 3, tile.height, tile.width};

            // Run Inference
            std::vector<float> outputData = session_->Run(inputData, inputDims);
            if (outputData.empty()) {
                std::cerr << "Inference returned empty data for tile." << std::endl;
                continue; 
            }

            // Post-process tile
            // Output dims should be [1, 3, H*scale, W*scale]
            int outTileH = tile.height * scale;
            int outTileW = tile.width * scale;
            
            cv::Mat resultTile = ImageUtils::PostProcess(outputData.data(), 3, outTileH, outTileW);

            // Place into canvas
            // We need to map original tile coordinates to scaled coordinates
            int targetX = tile.x * scale;
            int targetY = tile.y * scale;
            
            // Simple copy for now (no blending yet)
            // Be careful with boundaries
            if (targetX + outTileW <= outW && targetY + outTileH <= outH) {
                resultTile.copyTo(canvas(cv::Rect(targetX, targetY, outTileW, outTileH)));
            } else {
                // Crop if necessary (shouldn't happen if logic is correct)
                int w = std::min(outTileW, outW - targetX);
                int h = std::min(outTileH, outH - targetY);
                resultTile(cv::Rect(0,0,w,h)).copyTo(canvas(cv::Rect(targetX, targetY, w, h)));
            }
        }

        // Optional: Sharpen
        if (options_.strength > 0) {
            canvas = ImageUtils::Sharpen(canvas, options_.strength);
        }

        return canvas;
    }

}
