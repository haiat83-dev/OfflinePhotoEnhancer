# Hướng dẫn cài đặt Visual Studio để build project

## Vấn đề

Máy tính chưa có **Visual Studio**, cần thiết để compile C++ code thành file .exe.

## Giải pháp

### Lựa chọn 1: Cài Visual Studio Community (Miễn phí - Khuyến nghị)

1. Tải Visual Studio Community 2022:
   https://visualstudio.microsoft.com/downloads/

2. Khi cài đặt, chọn workload: **"Desktop development with C++"**
   - Tích chọn: C++ CMake tools for Windows
   - Tích chọn: C++ ATL for latest v143 build tools

3. Sau khi cài xong, chạy lại:
   ```cmd
   build_windows.bat
   ```

### Lựa chọn 2: Sử dụng Build Tools for Visual Studio (Nhẹ hơn)

Nếu không muốn cài full VS:

1. Tải Build Tools:
   https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022

2. Chọn workload: **"C++ build tools"**
   - Tích chọn: MSVC v143 - VS 2022 C++ x64/x86 build tools
   - Tích chọn: Windows SDK
   - Tích chọn: CMake tools

3. Chạy lại build script

### Lựa chọn 3: Nhờ người khác build hoặc dùng GitHub Actions

Nếu không thể/không muốn cài VS:
- Commit code lên GitHub
- Tôi có thể tạo GitHub Actions workflow để tự động build trên cloud
- Sau đó tải file .exe về

## Tóm tắt

**Không thể build C++ mà không có compiler (MSVC).**
Visual Studio là cách dễ nhất để có compiler trên Windows.

Bạn muốn chọn cách nào?
