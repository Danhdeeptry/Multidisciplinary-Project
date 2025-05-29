import sqlite3
import os
import json
import sys
import time
from datetime import datetime

def init_db():
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
    
    conn.commit()
    return conn, cursor

def query_database(device_name=None, limit=0):
    conn, cursor = init_db()
    
    # Xây dựng câu truy vấn
    if device_name:
        sql = 'SELECT id, device_name, timestamp, data_json FROM telemetry_data WHERE device_name = ? ORDER BY id DESC'
        params = (device_name,)
    else:
        sql = 'SELECT id, device_name, timestamp, data_json FROM telemetry_data ORDER BY id DESC'
        params = ()
    if limit > 0:
        sql += f' LIMIT {limit}'
    
    cursor.execute(sql, params)
    rows = cursor.fetchall()
    
    print(f"Số lượng bản ghi tìm thấy: {len(rows)}")
    for row in rows:
        id, device, timestamp, data_json = row
        data = json.loads(data_json)
        print(f"ID: {id}, Device: {device}, Time: {timestamp}")
        print(f"Data: {data}\n")
    
    conn.close()

def main():
    import argparse
    parser = argparse.ArgumentParser(description="Xem dữ liệu telemetry theo device và real-time nếu muốn.")
    parser.add_argument('--device', type=str, default=None, help='Tên thiết bị (device_name) muốn xem. Nếu bỏ trống sẽ lấy tất cả.')
    parser.add_argument('--limit', type=int, default=10, help='Số lượng bản ghi muốn lấy (0 = không giới hạn)')
    parser.add_argument('--watch', action='store_true', help='Bật chế độ theo dõi real-time (auto-refresh)')
    parser.add_argument('--interval', type=int, default=2, help='Thời gian refresh (giây) khi ở chế độ watch')
    args = parser.parse_args()

    if args.watch:
        print("Đang theo dõi dữ liệu real-time. Nhấn Ctrl+C để dừng.")
        last_count = 0
        while True:
            os.system('cls' if os.name == 'nt' else 'clear')
            print(f"Thời gian: {datetime.now().strftime('%H:%M:%S')}")
            query_database(args.device, args.limit)
            time.sleep(args.interval)
    else:
        query_database(args.device, args.limit)

if __name__ == "__main__":
    main()
