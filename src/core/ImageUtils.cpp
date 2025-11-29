#include "ImageUtils.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace Core {

    cv::Mat ImageUtils::LoadImage(const std::wstring& path) {
        // OpenCV imread doesn't support unicode paths on Windows directly in all versions.
        // Use a buffer approach.
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return cv::Mat();

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size)) {
            return cv::imdecode(buffer, cv::IMREAD_COLOR);
        }
        return cv::Mat();
    }

    bool ImageUtils::SaveImage(const std::wstring& path, const cv::Mat& image) {
        std::vector<uchar> buf;
        std::string ext = "png"; // Default
        size_t dotPos = path.find_last_of(L'.');
        if (dotPos != std::string::npos) {
            std::wstring wExt = path.substr(dotPos + 1);
            // Simple conversion for extension check
            std::string sExt(wExt.begin(), wExt.end()); 
            ext = sExt;
        }

        if (cv::imencode("." + ext, image, buf)) {
            std::ofstream file(path, std::ios::binary);
            if (file.is_open()) {
                file.write(reinterpret_cast<const char*>(buf.data()), buf.size());
                return true;
            }
        }
        return false;
    }

    std::vector<float> ImageUtils::PreProcess(const cv::Mat& img) {
        cv::Mat rgb;
        cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
        
        // Convert to float and normalize [0, 1]
        cv::Mat float_img;
        rgb.convertTo(float_img, CV_32F, 1.0 / 255.0);

        // HWC to CHW
        std::vector<float> output;
        output.reserve(img.channels() * img.rows * img.cols);

        std::vector<cv::Mat> channels;
        cv::split(float_img, channels);

        for (const auto& c : channels) {
            output.insert(output.end(), (float*)c.datastart, (float*)c.dataend);
        }

        return output;
    }

    cv::Mat ImageUtils::PostProcess(const float* outputData, int channels, int height, int width) {
        std::vector<cv::Mat> cv_channels;
        int channel_size = height * width;

        for (int i = 0; i < channels; ++i) {
            cv::Mat c(height, width, CV_32F, const_cast<float*>(outputData + i * channel_size));
            cv_channels.push_back(c);
        }

        cv::Mat rgb;
        cv::merge(cv_channels, rgb);

        // Clip to [0, 1]
        cv::threshold(rgb, rgb, 1.0, 1.0, cv::THRESH_TRUNC);
        cv::threshold(rgb, rgb, 0.0, 0.0, cv::THRESH_TOZERO);

        cv::Mat bgr;
        cv::cvtColor(rgb, bgr, cv::COLOR_RGB2BGR);

        cv::Mat final_img;
        bgr.convertTo(final_img, CV_8U, 255.0);
        return final_img;
    }

    std::vector<ImageTile> ImageUtils::SplitTiles(const cv::Mat& img, int tile_size, int overlap) {
        std::vector<ImageTile> tiles;
        int h = img.rows;
        int w = img.cols;

        for (int y = 0; y < h; y += tile_size - overlap) {
            for (int x = 0; x < w; x += tile_size - overlap) {
                int tw = std::min(tile_size, w - x);
                int th = std::min(tile_size, h - y);
                
                // Adjust for boundary to keep tile size constant if possible (optional, but good for batching)
                // For now, simple cropping
                cv::Rect roi(x, y, tw, th);
                tiles.push_back({img(roi).clone(), x, y, tw, th});
            }
        }
        return tiles;
    }

    cv::Mat ImageUtils::MergeTiles(const std::vector<ImageTile>& tiles, int full_width, int full_height, int tile_size, int overlap) {
        // This is a simplified merge. For better results, we should blend the overlap areas.
        // Current implementation: Just overwrite. 
        // NOTE: A proper implementation would use a weight mask for blending.
        
        cv::Mat canvas = cv::Mat::zeros(full_height, full_width, CV_8UC3);
        
        // Naive merge: just paste. 
        // TODO: Implement feathering/blending if artifacts are visible.
        for (const auto& tile : tiles) {
            // We need to handle the fact that tiles might be scaled up (e.g. x2, x4)
            // But this function signature assumes we know the target full size.
            // The tile.x and tile.y are in ORIGINAL coordinates.
            // We need to know the scale factor to place them correctly.
            
            // Infer scale from tile vs original tile size? 
            // Or just assume the caller handles coordinate mapping?
            // Let's assume tile.x/y are TARGET coordinates for this function to be generic for the canvas.
            // Wait, SplitTiles returns original coords. 
            // So we need to scale coords.
            
            // Let's change the logic: The caller should probably manage the canvas or we pass scale.
            // For now, let's assume the passed tiles have their x/y updated to target coordinates 
            // OR we calculate scale here.
            
            // Actually, let's make it simple: 
            // If we have a 100x100 tile at 0,0 and it becomes 400x400, the new pos is 0,0.
            // If we have a tile at 50,0 (overlap), it becomes 200,0.
            
            // Let's assume the caller will assemble. 
            // But for now, let's just implement a direct paste based on tile.x/y * scale.
            
            // We can deduce scale.
            // But wait, tile.width/height are the CURRENT tile size.
            // We don't have the original tile size here easily unless we pass it.
            
            // Let's skip complex merge for this step and refine in ImageProcessor.
            // Just paste:
            if (tile.x + tile.width <= full_width && tile.y + tile.height <= full_height) {
                 tile.data.copyTo(canvas(cv::Rect(tile.x, tile.y, tile.width, tile.height)));
            }
        }
        return canvas;
    }

    cv::Mat ImageUtils::Sharpen(const cv::Mat& img, double strength) {
        if (strength <= 0) return img.clone();
        
        cv::Mat blurred, weighted;
        cv::GaussianBlur(img, blurred, cv::Size(0, 0), 3);
        cv::addWeighted(img, 1.0 + strength, blurred, -strength, 0, weighted);
        return weighted;
    }

}
