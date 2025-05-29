import sqlite3
import os

# Tạo thư mục data nếu chưa tồn tại
os.makedirs('data', exist_ok=True)

# Kết nối database
conn = sqlite3.connect('data/sensor_data.db')
cursor = conn.cursor()

# Tạo bảng nếu chưa tồn tại
cursor.execute('''
CREATE TABLE IF NOT EXISTS telemetry_data (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_name TEXT NOT NULL,
    timestamp TEXT NOT NULL,
    data_json TEXT NOT NULL
)
''')

# Thêm một số dữ liệu mẫu
import json
from datetime import datetime

for i in range(5):
    device_name = "test_device"
    timestamp = datetime.now().isoformat()
    temp = 25 + i
    humidity = 60 + i
    data = {
        "temperature": temp,
        "humidity": humidity
    }
    data_json = json.dumps(data)
    
    cursor.execute(
        "INSERT INTO telemetry_data (device_name, timestamp, data_json) VALUES (?, ?, ?)",
        (device_name, timestamp, data_json)
    )

# Lưu thay đổi
conn.commit()

# Hiển thị dữ liệu
cursor.execute('SELECT * FROM telemetry_data ORDER BY id DESC LIMIT 10')
print("Dữ liệu trong cơ sở dữ liệu:")
for row in cursor.fetchall():
    print(row)

conn.close()
print("\nĐã tạo và cập nhật cơ sở dữ liệu thành công!")
