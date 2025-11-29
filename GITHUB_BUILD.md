# Hướng dẫn tự động build bằng GitHub Actions

## Các bước thực hiện:

### 1. Tạo repository trên GitHub
- Vào https://github.com/new
- Đặt tên repo (ví dụ: `OfflinePhotoEnhancer`)
- Chọn Public hoặc Private
- Không tích "Initialize with README"
- Click "Create repository"

### 2. Push code lên GitHub
Chạy các lệnh sau trong PowerShell tại folder `E:\ToolNetAnh`:

```powershell
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
git add .
git commit -m "Initial commit - Offline Photo Enhancer"
git remote add origin https://github.com/YOUR_USERNAME/OfflinePhotoEnhancer.git
git push -u origin feature/engine-cli
```

**Thay `YOUR_USERNAME` bằng username GitHub của bạn!**

### 3. Đợi GitHub Actions build
- Vào repo trên GitHub
- Click tab "Actions"
- Sẽ thấy workflow "Build Windows Application" đang chạy
- Đợi khoảng 5-10 phút

### 4. Tải file exe về
- Sau khi build xong (có dấu ✅ xanh)
- Click vào workflow run
- Scroll xuống phần "Artifacts"
- Tải file `OfflinePhotoEnhancer-Windows-x64.zip`
- Giải nén và chạy!

## Lưu ý
- Lần đầu chạy có thể lâu vì phải tải OpenCV (~500MB)
- Mỗi lần bạn push code mới, Actions sẽ tự build lại
- Không tốn tiền gì cả (GitHub Actions miễn phí cho public repo)

## Nếu gặp lỗi
- Kiểm tra tab Actions để xem log chi tiết
- Có thể cần enable Actions trong repo settings
