@echo off
echo Setting up dependencies...
echo.
echo 1. Please download OpenCV 4.x (Windows) and extract to third_party/opencv
echo    https://opencv.org/releases/
echo.
echo 2. Please download ONNX Runtime (Windows x64) and extract to third_party/onnxruntime
echo    https://github.com/microsoft/onnxruntime/releases
echo    (Ensure you get the version compatible with your GPU drivers if using CUDA/DirectML)
echo.
echo Structure should look like:
echo   third_party/opencv/build/include/...
echo   third_party/onnxruntime/include/...
echo   third_party/onnxruntime/lib/onnxruntime.lib
echo.
pause
