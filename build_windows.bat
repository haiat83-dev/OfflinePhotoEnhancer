@echo off
setlocal enabledelayedexpansion

echo [INFO] Checking dependencies...
if not exist "third_party\opencv\build" (
    echo [WARN] OpenCV not found. Attempting to download...
    powershell -ExecutionPolicy Bypass -File scripts\download_deps.ps1
    if errorlevel 1 (
        echo [ERROR] Failed to download dependencies.
        pause
        exit /b 1
    )
)

echo [INFO] Locating Visual Studio...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if "%VS_PATH%"=="" (
    echo [ERROR] Visual Studio with C++ tools not found.
    pause
    exit /b 1
)

echo [INFO] Found VS at: %VS_PATH%
set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
    echo [ERROR] vcvars64.bat not found at expected location.
    pause
    exit /b 1
)

call "%VCVARS%"

:: Check CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [INFO] CMake not in PATH. Searching in VS...
    if exist "%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
        set "PATH=%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%"
        echo [INFO] Added VS CMake to PATH.
    ) else (
        echo [ERROR] CMake not found.
        pause
        exit /b 1
    )
)

echo [INFO] Building...
if not exist build mkdir build
cd build

cmake ..
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo [INFO] Packaging...
cd ..
powershell -ExecutionPolicy Bypass -File scripts\package.ps1

echo [SUCCESS] Build complete. Run dist/OfflinePhotoEnhancer/enhancer-gui.exe
pause
