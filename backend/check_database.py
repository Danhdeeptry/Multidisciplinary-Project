from pymongo import MongoClient
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
    
def connect_mongo(uri="mongodb://localhost:27017", db_name="YoloFarms", collection_name="YoloFarmsData"):
    client = MongoClient(uri)
    db = client[db_name]
    return db[collection_name]

def push_sqlite_to_mongo(uri="mongodb://localhost:27017", db_name="YoloFarms", collection_name="YoloFarmsData"):
    # Connect to MongoDB
    client = MongoClient(uri)
    mongo_collection = client[db_name][collection_name]
    
    # Connect to SQLite
    conn = sqlite3.connect('data/sensor_data.db')
    cursor = conn.cursor()

    # Fetch all rows
    cursor.execute("SELECT data_json FROM telemetry_data")
    rows = cursor.fetchall()

    # Convert and insert only data_json
    inserted_count = 0
    for (data_json_str,) in rows:
        try:
            data = json.loads(data_json_str)  # Convert JSON string to dictionary
            if isinstance(data, dict):
                mongo_collection.insert_one(data)
                inserted_count += 1
        except json.JSONDecodeError:
            print("Lỗi giải mã JSON:", data_json_str)

    conn.close()
    print(f"Đã đẩy {inserted_count} bản ghi (chỉ data) vào MongoDB.")

def print_mongo_data(limit=10):
    mongo_collection = connect_mongo()
    
    print(f"Đang in ra tối đa {limit} bản ghi từ MongoDB...\n")
    cursor = mongo_collection.find().sort("_id", -1).limit(limit)
    
    count = 0
    for doc in cursor:
        count += 1
        print(f"ID: {doc['_id']}")
        print(f"Device: {doc['device_name']}")
        print(f"Time: {doc['timestamp']}")
        print(f"Data: {json.dumps(doc['data'], indent=2)}")
        print("-" * 40)
    
    print(f"Tổng số bản ghi in ra: {count}")

def get_latest_mongo_id(mongo_collection):
    latest_doc = mongo_collection.find_one(sort=[("_id", -1)])
    return latest_doc["_id"] if latest_doc else 0

def live_sync_sqlite_to_mongo(uri="mongodb://localhost:27017", db_name="YoloFarms", collection_name="YoloFarmsData", poll_interval=3):
    # conn, cursor = init_db()
    # mongo_collection = connect_mongo()

    # last_id = get_latest_mongo_id(mongo_collection)
    # print("Bắt đầu đồng bộ real-time từ ID... (Nhấn Ctrl+C để dừng)")

    # try:
    #     while True:
    #         sql = 'SELECT id, device_name, timestamp, data_json FROM telemetry_data WHERE id > ? ORDER BY id ASC'
    #         cursor.execute(sql, (last_id,))
    #         rows = cursor.fetchall()

    #         for row in rows:
    #             record_id, device_name, timestamp, data_json = row
    #             doc = {
    #                 "_id": record_id,
    #                 "device_name": device_name,
    #                 "timestamp": timestamp,
    #                 "data": json.loads(data_json)
    #             }

    #             try:
    #                 mongo_collection.insert_one(doc)
    #                 print(f"Đã insert ID {record_id} vào MongoDB.")
    #                 last_id = record_id
    #             except Exception as e:
    #                 print(f"Lỗi khi insert ID {record_id}: {e}")

    #         time.sleep(poll_interval)
    # except KeyboardInterrupt:
    #     print("Dừng đồng bộ real-time.")
    # finally:
    #     conn.close()
    # Connect to MongoDB
    client = MongoClient(uri)
    mongo_collection = client[db_name][collection_name]
    
    # Connect to SQLite
    conn = sqlite3.connect('data/sensor_data.db')
    cursor = conn.cursor()

    # Fetch all rows
    cursor.execute("SELECT data_json FROM telemetry_data")
    rows = cursor.fetchone()
    
    try:
        while True:
            data_json = rows[0]
            try:
                data = json.loads(data_json)  # Convert JSON string to dictionary
                if isinstance(data, dict):
                    mongo_collection.insert_one(data)
                time.sleep(3)
            except json.JSONDecodeError:
                print("Lỗi giải mã JSON:", data_json)
    except KeyboardInterrupt:
        print("Dừng đồng bộ real-time.")
    finally:
        conn.close()
        
# connect_mongo()
# push_sqlite_to_mongo()
# live_sync_sqlite_to_mongo()


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
