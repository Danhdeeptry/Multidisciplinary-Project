# Hướng dẫn sử dụng Mô phỏng IoT với ThingsBoard

## Giới thiệu

Các công cụ mô phỏng này được tạo ra để giúp bạn phát triển và thử nghiệm hệ thống IoT mà không cần thiết bị thực (ESP32). Bạn có thể sử dụng những công cụ này để:

1. Kiểm tra kết nối với ThingsBoard
2. Mô phỏng việc gửi dữ liệu từ thiết bị lên ThingsBoard
3. Tạo giao diện để theo dõi và điều khiển thiết bị ảo
4. Thử nghiệm backend nhận dữ liệu từ ThingsBoard

## Cài đặt yêu cầu

```bash
pip install requests tkinter
```

## Các công cụ mô phỏng

### 1. Kiểm tra kết nối ThingsBoard

Script `test_connection.py` giúp bạn kiểm tra kết nối đến server ThingsBoard trước khi sử dụng công cụ mô phỏng.

Cách sử dụng:
```bash
python test_connection.py [host] [token]
```

Nếu không cung cấp tham số, script sẽ sử dụng giá trị mặc định:
- Host: app.coreiot.io
- Token: lr3yyoh7v2b98pdmuj38

### 2. Mô phỏng gửi dữ liệu trực tiếp

Script `device_simulator.py` mô phỏng một thiết bị IoT gửi dữ liệu telemetry và attributes lên ThingsBoard theo định kỳ.

Cách sử dụng:
```bash
python device_simulator.py
```

Bạn có thể chỉnh sửa các tham số trong file như:
- THINGSBOARD_HOST: Địa chỉ máy chủ ThingsBoard
- ACCESS_TOKEN: Token của thiết bị
- interval_seconds: Khoảng thời gian giữa các lần gửi dữ liệu
- total_time_minutes: Tổng thời gian chạy mô phỏng

### 3. Giao diện mô phỏng ESP32

Script `esp32_gui_simulator.py` tạo một giao diện người dùng trực quan để theo dõi và điều khiển thiết bị IoT ảo.

Cách sử dụng:
```bash
python esp32_gui_simulator.py
```

Giao diện này cho phép bạn:
- Kết nối/ngắt kết nối với ThingsBoard
- Điều chỉnh nhiệt độ và độ ẩm bằng thanh trượt
- Bật/tắt LED ảo
- Theo dõi quá trình gửi dữ liệu qua cửa sổ terminal

### 4. Chạy Backend nhận dữ liệu

Để khởi chạy backend Node.js đã được cấu hình trong phần trước:

```bash
cd simple_server+backend
python run_server.py
```

## Luồng dữ liệu đầy đủ

Để thử nghiệm toàn bộ luồng dữ liệu, bạn có thể làm theo các bước sau:

1. Kiểm tra kết nối ThingsBoard:
   ```
   python simulator\test_connection.py
   ```

2. Khởi động backend để nhận dữ liệu từ ThingsBoard:
   ```
   cd simple_server+backend
   python run_server.py
   ```

3. Nhập token Ngrok khi được yêu cầu và lưu URL công khai được cung cấp

4. Đăng nhập vào ThingsBoard và cấu hình Rule Chain để chuyển tiếp dữ liệu đến URL Ngrok

5. Chạy một trong các mô phỏng thiết bị:
   ```
   python simulator\device_simulator.py
   ```
   hoặc
   ```
   python simulator\esp32_gui_simulator.py
   ```

6. Kiểm tra dữ liệu trong:
   - Dashboard ThingsBoard
   - Terminal của backend
   - Database SQLite (data/sensor_data.db)

## Xác thực và gỡ lỗi

Để xem dữ liệu đã lưu trong database:

```bash
cd simple_server+backend
python -c "import sqlite3; conn=sqlite3.connect('data/sensor_data.db'); cursor=conn.cursor(); cursor.execute('SELECT * FROM telemetry_data ORDER BY id DESC LIMIT 10'); print(cursor.fetchall()); conn.close()"
```

Để xem API Swagger của backend:

```
http://localhost:8000/docs
```
hoặc qua URL Ngrok:
```
https://xxxx-xx-xx-xxx-xx.ngrok-free.app/docs
```
