import requests

url = "https://app.coreiot.io/api/auth/login"

payload = {
    "username": "vanduyanh2016@gmail.com",
    "password": "anhcrazy2606:))"
}

response = requests.post(url, json=payload)
token = response.json()["token"]
# print(token)

headers = {
    "X-Authorization": f"Bearer {token}"
}

# device_name = "D1_Multidisciplinary_Project"
# resp = requests.get(f"https://app.coreiot.io/api/tenant/devices?deviceName={device_name}", headers=headers)
# device_id = resp.json()
# print(device_id)

url = f"https://app.coreiot.io/api/plugins/telemetry/DEVICE/71832ad0-1766-11f0-b943-e12b9c63441a/values/timeseries?keys=temperature,humidity"

# response = requests.get(url, headers=headers)
# data = response.json()
# print(data)

import sqlite3
import requests
import time
from pymongo import MongoClient
from datetime import datetime

# Replace with your actual public device token if you have one
PUBLIC_DEVICE_TOKEN = "c91668c0-e48f-11ef-b7af-d3ad854234f3"

# URL to pull telemetry (this pulls all keys, adjust if needed)
BASE_URL = f"https://app.coreiot.io/api/plugins/telemetry/DEVICE/71832ad0-1766-11f0-b943-e12b9c63441a/values/timeseries?keys=temperature,humidity"

# SQLite database setup
# DB_NAME = "data/sensor_data.db"

# def create_table():
#     conn = sqlite3.connect(DB_NAME)
#     c = conn.cursor()
#     c.execute('''
#         CREATE TABLE IF NOT EXISTS telemetry (
#             id INTEGER PRIMARY KEY AUTOINCREMENT,
#             timestamp TEXT,
#             temperature REAL,
#             humidity REAL
#         )
#     ''')
#     conn.commit()
#     conn.close()

# def insert_data(timestamp, temperature, humidity):
#     conn = sqlite3.connect(DB_NAME)
#     c = conn.cursor()
#     c.execute("INSERT INTO telemetry (timestamp, temperature, humidity) VALUES (?, ?, ?)",
#               (timestamp, temperature, humidity))
#     conn.commit()
#     conn.close()
    
client = MongoClient("mongodb://localhost:27017")
db = client["YoloFarms"]
collection = db["YoloFarmsData"]

def pull_telemetry():
    try:
        response = requests.get(url, headers=headers)
        if response.status_code == 200:
            data = response.json()
            temperature = float(data.get('temperature', [{}])[0].get('value', 0))
            humidity = float(data.get('humidity', [{}])[0].get('value', 0))
            timestamp = datetime.now().isoformat()

            print(f"Pulled at {timestamp}: Temp={temperature}, Humidity={humidity}")
            document = {
                "temperature": float(temperature) if temperature else None,
                "humidity": float(humidity) if humidity else None,
            }
            collection.insert_one(document)
            print("Data inserted into MongoDB\n")
        else:
            print(f"Failed to fetch telemetry. Status: {response.status_code}, Response: {response.text}")
    except Exception as e:
        print(f"Error pulling telemetry: {e}")

if __name__ == "__main__":
    print("Starting telemetry pull every 0.1 seconds...")
    while True:
        pull_telemetry()
        time.sleep(0.1)