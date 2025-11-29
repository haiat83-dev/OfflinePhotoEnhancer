Offline Photo Enhancer (Windows x64)
======================================

A lightweight, offline tool for upscaling and enhancing photos using AI.

Requirements
------------
- Windows 10/11 (64-bit)
- Visual C++ Redistributable 2019+
- (Optional) NVIDIA GPU with latest drivers for CUDA acceleration.

Installation
------------
1. Extract the zip file to a folder.
2. Place your `.onnx` model files in the `models/` folder.
   (Note: A `stub.onnx` is included for testing, but it does not perform real upscaling).

Usage (GUI)
-----------
1. Run `enhancer-gui.exe`.
2. Drag and drop images into the window.
3. Select Scale (2x or 4x).
4. Click "Start Processing".

Usage (CLI)
-----------
Run `enhancer-cli.exe` from Command Prompt or PowerShell:

    enhancer-cli.exe --input "photo.jpg" --output "photo_upscaled.png" --model "models/RealESRGAN_x4.onnx" --scale 4

Options:
  --input <path>      Path to input image or directory (if --batch used)
  --output <path>     Path to output file or directory
  --model <path>      Path to ONNX model file
  --scale <2|4>       Upscale factor
  --device <cpu|dml>  Use CPU or DirectML (GPU)
  --batch             Enable batch processing for directories

Models
------
This application supports standard Super-Resolution ONNX models (e.g., Real-ESRGAN, SwinIR) converted to ONNX.
Input shape should be dynamic or fixed tile size.

License
-------
[Your License Here]
Uses OpenCV and ONNX Runtime.
