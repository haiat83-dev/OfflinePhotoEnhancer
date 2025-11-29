#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

namespace Core {

    struct ImageTile {
        cv::Mat data;
        int x, y; // Original coordinates
        int width, height;
    };

    class ImageUtils {
    public:
        // Load image from path. Supports unicode paths on Windows.
        static cv::Mat LoadImage(const std::wstring& path);

        // Save image to path.
        static bool SaveImage(const std::wstring& path, const cv::Mat& image);

        // Pre-process: Convert BGR (OpenCV default) to RGB, normalize to [0, 1], and convert to CHW float format.
        // Returns a flat vector of floats.
        static std::vector<float> PreProcess(const cv::Mat& img);

        // Post-process: Convert CHW float format back to BGR [0, 255] uint8.
        static cv::Mat PostProcess(const float* outputData, int channels, int height, int width);

        // Split image into tiles with overlap.
        static std::vector<ImageTile> SplitTiles(const cv::Mat& img, int tile_size, int overlap);

        // Merge tiles back into a single image.
        static cv::Mat MergeTiles(const std::vector<ImageTile>& tiles, int full_width, int full_height, int tile_size, int overlap);
        
        // Simple sharpening using unsharp mask
        static cv::Mat Sharpen(const cv::Mat& img, double strength);
    };

}
