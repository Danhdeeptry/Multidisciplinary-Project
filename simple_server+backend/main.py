from fastapi import FastAPI
from pydantic import BaseModel
from typing import Any, Dict
import sqlite3
from datetime import datetime
import os
import json

app = FastAPI()

# Khởi tạo cơ sở dữ liệu
def init_db():
    # Tạo thư mục data nếu chưa tồn tại
    os.makedirs('data', exist_ok=True)
    
    conn = sqlite3.connect('data/sensor_data.db')
    cursor = conn.cursor()
    
    # Tạo bảng lưu dữ liệu telemetry
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS telemetry_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        device_name TEXT NOT NULL,
        timestamp TEXT NOT NULL,
        data_json TEXT NOT NULL
    )
    ''')
    
    conn.commit()
    conn.close()

# Gọi hàm khởi tạo khi ứng dụng khởi động
init_db()

class TBMessage(BaseModel):
    deviceName: str
    telemetry: Dict[str, Any]

@app.post("/thingsboard-data")
def receive_tb_data(payload: TBMessage):
    # In log để debug
    print("✅ Received from ThingsBoard:", payload)
    
    # Lưu payload vào database
    try:
        conn = sqlite3.connect('data/sensor_data.db')
        cursor = conn.cursor()
        
        # Chuyển đổi dữ liệu telemetry thành chuỗi JSON
        telemetry_json = json.dumps(payload.telemetry)
        
        # Lấy timestamp hiện tại
        current_time = datetime.now().isoformat()
        
        # Thêm dữ liệu vào database
        cursor.execute(
            "INSERT INTO telemetry_data (device_name, timestamp, data_json) VALUES (?, ?, ?)",
            (payload.deviceName, current_time, telemetry_json)
        )
        
        conn.commit()
        conn.close()
        print("✅ Data saved to database successfully")
    except Exception as e:
        print(f"❌ Error saving to database: {str(e)}")
    
    return {"status": "ok"}

# Thêm endpoint để truy xuất dữ liệu
@app.get("/data/{device_name}")
def get_device_data(device_name: str, limit: int = 50):
    try:
        conn = sqlite3.connect('data/sensor_data.db')
        cursor = conn.cursor()
        
        cursor.execute(
            "SELECT timestamp, data_json FROM telemetry_data WHERE device_name = ? ORDER BY id DESC LIMIT ?",
            (device_name, limit)
        )
        
        results = []
        for row in cursor.fetchall():
            timestamp, data_json = row
            data = {
                "timestamp": timestamp,
                "data": json.loads(data_json)
            }
            results.append(data)
        
        conn.close()
        return {"device": device_name, "data": results}
    except Exception as e:
        return {"error": str(e)}


#lệnh chạy
# uvicorn main:app --host 0.0.0.0 --port 8000 --reload
