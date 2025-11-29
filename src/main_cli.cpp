#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "core/Engine.hpp"

// Simple argument parsing helper
struct Args {
    std::wstring input;
    std::wstring output;
    std::wstring model;
    int scale = 4;
    Core::Device device = Core::Device::CPU;
    bool batch = false;
};

void print_usage() {
    std::cout << "Usage: enhancer-cli --input <path> --output <path> --model <path> [options]\n"
              << "Options:\n"
              << "  --scale <2|4>       Upscale factor (default: 4)\n"
              << "  --device <cpu|dml>  Inference device (default: cpu)\n"
              << "  --batch             Treat input as directory\n";
}

Args parse_args(int argc, char* argv[]) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            std::string val = argv[++i];
            args.input = std::wstring(val.begin(), val.end());
        } else if (arg == "--output" && i + 1 < argc) {
            std::string val = argv[++i];
            args.output = std::wstring(val.begin(), val.end());
        } else if (arg == "--model" && i + 1 < argc) {
            std::string val = argv[++i];
            args.model = std::wstring(val.begin(), val.end());
        } else if (arg == "--scale" && i + 1 < argc) {
            args.scale = std::stoi(argv[++i]);
        } else if (arg == "--device" && i + 1 < argc) {
            std::string val = argv[++i];
            if (val == "dml" || val == "cuda") args.device = Core::Device::DirectML; // Map cuda to DML for now or separate
            else args.device = Core::Device::CPU;
        } else if (arg == "--batch") {
            args.batch = true;
        }
    }
    return args;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        print_usage();
        return 1;
    }

    Args args = parse_args(argc, argv);

    Core::EngineOptions opts;
    opts.modelPath = args.model;
    opts.scale = args.scale;
    opts.device = args.device;

    Core::Engine engine(opts);
    
    std::cout << "Initializing engine..." << std::endl;
    if (!engine.Initialize()) {
        std::cerr << "Engine initialization failed." << std::endl;
        return 1;
    }

    if (args.batch) {
        // Collect files
        std::vector<std::wstring> files;
        for (const auto& entry : std::filesystem::directory_iterator(args.input)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().wstring());
            }
        }
        
        engine.ProcessBatch(files, args.output, [](const Core::ProgressEvent& evt) {
            std::cout << "[" << evt.currentFileIndex << "/" << evt.totalFiles << "] " 
                      << (int)(evt.percentComplete * 100) << "% - " << evt.statusMessage << "\r";
        });
        std::cout << "\nBatch processing complete." << std::endl;

    } else {
        std::cout << "Processing file..." << std::endl;
        if (engine.ProcessFile(args.input, args.output)) {
            std::cout << "Success!" << std::endl;
        } else {
            std::cerr << "Failed to process file." << std::endl;
            return 1;
        }
    }

    return 0;
}
