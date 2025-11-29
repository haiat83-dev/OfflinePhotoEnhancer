#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "../src/core/ImageUtils.hpp"

void test_tiling() {
    std::cout << "Testing Tiling..." << std::endl;
    // Create a 100x100 dummy image
    cv::Mat img = cv::Mat::zeros(100, 100, CV_8UC3);
    
    int tile_size = 32;
    int overlap = 4;
    
    auto tiles = Core::ImageUtils::SplitTiles(img, tile_size, overlap);
    
    // Check if we have tiles
    assert(!tiles.empty());
    
    // Check first tile
    assert(tiles[0].x == 0);
    assert(tiles[0].y == 0);
    assert(tiles[0].width == 32);
    assert(tiles[0].height == 32);
    
    std::cout << "Tiling OK. Generated " << tiles.size() << " tiles." << std::endl;
}

void test_preprocess() {
    std::cout << "Testing Preprocess..." << std::endl;
    cv::Mat img(2, 2, CV_8UC3, cv::Scalar(0, 0, 255)); // Red image (BGR: 0, 0, 255)
    
    auto data = Core::ImageUtils::PreProcess(img);
    
    // Size should be 3 channels * 2 * 2 = 12
    assert(data.size() == 12);
    
    // Check value: Red channel (index 0 in RGB) should be 1.0
    // PreProcess converts BGR -> RGB. So R is first channel.
    // data layout: RRRR GGGG BBBB
    // R channel is first 4 elements.
    assert(std::abs(data[0] - 1.0f) < 1e-5);
    
    std::cout << "Preprocess OK." << std::endl;
}

int main() {
    test_tiling();
    test_preprocess();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
