$ErrorActionPreference = "Stop"
$rootDir = Get-Location
$thirdPartyDir = "$rootDir\third_party"

if (-not (Test-Path $thirdPartyDir)) { New-Item -ItemType Directory -Force -Path $thirdPartyDir | Out-Null }

# URLs - Using zip files for easier extraction
$opencvUrl = "https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe"
$onnxUrl = "https://github.com/microsoft/onnxruntime/releases/download/v1.16.3/onnxruntime-win-x64-1.16.3.zip"

# OpenCV - Try to extract from exe or provide manual instructions
$opencvDir = "$thirdPartyDir\opencv"
if (-not (Test-Path "$opencvDir\build")) {
    Write-Host "OpenCV not found."
    Write-Host "Please download OpenCV manually from: $opencvUrl"
    Write-Host "Extract it to: $thirdPartyDir"
    Write-Host "The structure should be: third_party\opencv\build\"
    Write-Host ""
    Write-Host "Alternatively, you can use a pre-built OpenCV zip if available."
    Read-Host "Press Enter after you've placed OpenCV, or Ctrl+C to cancel"
}

# ONNX Runtime
$onnxDir = "$thirdPartyDir\onnxruntime"
if (-not (Test-Path "$onnxDir\include\onnxruntime_cxx_api.h")) {
    Write-Host "Downloading ONNX Runtime..."
    $onnxZip = "$thirdPartyDir\onnxruntime.zip"
    
    try {
        Invoke-WebRequest -Uri $onnxUrl -OutFile $onnxZip -UseBasicParsing
        
        Write-Host "Extracting ONNX Runtime..."
        Expand-Archive -Path $onnxZip -DestinationPath $thirdPartyDir -Force
        
        # Rename folder (it usually has version in name)
        $extracted = Get-ChildItem -Path $thirdPartyDir -Directory -Filter "onnxruntime-win-x64*" | Select-Object -First 1
        if ($extracted) {
            if (Test-Path "$thirdPartyDir\onnxruntime") {
                Remove-Item "$thirdPartyDir\onnxruntime" -Recurse -Force
            }
            Rename-Item $extracted.FullName "onnxruntime"
        }
        
        Remove-Item $onnxZip -Force
        Write-Host "ONNX Runtime downloaded successfully."
    } catch {
        Write-Host "Failed to download ONNX Runtime: $_"
        Write-Host "Please download manually from: $onnxUrl"
        Write-Host "Extract to: $thirdPartyDir\onnxruntime"
    }
} else {
    Write-Host "ONNX Runtime already present."
}

Write-Host "Dependency check complete."
