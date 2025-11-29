$ErrorActionPreference = "Stop"

$rootDir = Get-Location
$buildDir = "$rootDir\build\bin"
$distDir = "$rootDir\dist\OfflinePhotoEnhancer"
$zipFile = "$rootDir\dist\OfflinePhotoEnhancer.zip"

Write-Host "Packaging Offline Photo Enhancer..."

# Clean dist
if (Test-Path $distDir) { Remove-Item -Recurse -Force $distDir }
if (Test-Path $zipFile) { Remove-Item -Force $zipFile }
New-Item -ItemType Directory -Force -Path $distDir | Out-Null
New-Item -ItemType Directory -Force -Path "$distDir\models" | Out-Null

# Copy Executables
$exes = @("enhancer-cli.exe", "enhancer-gui.exe")
foreach ($exe in $exes) {
    $src = "$buildDir\$exe"
    if (Test-Path $src) {
        Copy-Item $src $distDir
        Write-Host "Copied $exe"
    } elseif (Test-Path "$buildDir\Release\$exe") {
        Copy-Item "$buildDir\Release\$exe" $distDir
        Write-Host "Copied $exe (from Release)"
    } else {
        Write-Warning "Executable not found: $exe"
    }
}

# Copy DLLs (ONNX Runtime, OpenCV)
# We assume they are in the build bin folder (copied by CMake) or in third_party
# For this script, we look in build bin first
$dlls = Get-ChildItem "$buildDir\*.dll"
foreach ($dll in $dlls) {
    Copy-Item $dll.FullName $distDir
    Write-Host "Copied $($dll.Name)"
}

# Copy README and License
if (Test-Path "$rootDir\README.txt") { Copy-Item "$rootDir\README.txt" $distDir }
if (Test-Path "$rootDir\LICENSE") { Copy-Item "$rootDir\LICENSE" $distDir }

# Create Stub Model for testing
New-Item -ItemType File -Force -Path "$distDir\models\stub.onnx" | Out-Null

# Zip it
Write-Host "Creating Zip archive..."
Compress-Archive -Path "$distDir\*" -DestinationPath $zipFile

Write-Host "Packaging complete: $zipFile"
