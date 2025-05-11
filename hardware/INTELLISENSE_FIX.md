# Hướng dẫn khắc phục lỗi IntelliSense cho ESP32

Nếu bạn vẫn gặp lỗi IntelliSense với thông báo "cannot open source file" cho các thư viện ESP32, hãy làm theo các bước sau:

## 1. Khởi động lại IntelliSense Database

1. Mở Command Palette (Ctrl+Shift+P)
2. Nhập và chọn: `C/C++: Reset IntelliSense Database`
3. Sau đó khởi động lại VS Code: `Developer: Reload Window`

## 2. Chuyển sang Tag Parser

1. Mở Command Palette (Ctrl+Shift+P)
2. Nhập và chọn: `C/C++: Select IntelliSense Configuration`
3. Chọn `${workspaceFolder}/.vscode/c_cpp_properties.json`
4. Mở Command Palette lại (Ctrl+Shift+P)
5. Nhập và chọn: `C/C++: Change IntelliSense Engine`
6. Chọn `Tag Parser` để sử dụng phân tích đơn giản hơn

## 3. Tắt Squiggles nếu chúng làm phiền

Nếu không muốn thấy các lỗi, hãy:

1. Mở Command Palette (Ctrl+Shift+P)
2. Nhập và chọn: `C/C++: Toggle Error Squiggles`
3. Bây giờ chúng sẽ bị ẩn, nhưng code vẫn biên dịch được

## 4. Xác nhận môi trường

1. Nhấp vào lệnh `Reset IntelliSense` trong menu Tasks (Ctrl+Shift+P, "Run Task")
2. Chạy lệnh `pio run` để xác nhận rằng code vẫn biên dịch thành công

## 5. Sửa lỗi không tìm thấy các tệp header phụ thuộc 

1. File > Preferences > Settings
2. Tìm kiếm "C_Cpp: Intelli Sense Engine"
3. Thay đổi từ "Default" sang "Tag Parser"
4. Tìm kiếm "C_Cpp: Error Squiggles"
5. Thay đổi từ "Enabled" sang "Disabled" nếu vẫn thấy lỗi

> Lưu ý: IntelliSense đôi khi khó khăn với ESP32/Arduino, nhưng những lỗi này chỉ liên quan đến IDE và không ảnh hưởng đến khả năng biên dịch và chạy code thực tế. Chương trình của bạn đã biên dịch thành công. 