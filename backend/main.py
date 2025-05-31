from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pymongo import MongoClient
import asyncio
import threading

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Replace with frontend domain in prod
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Connect to MongoDB
client = MongoClient("mongodb://localhost:27017/")
db = client["Yolo"]
collection = db["YoloData"]

current_index = {"index": 0}
latest_15_data = []  # Cache of latest 15 documents

def cycle_data_index():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    
    async def toggle():
        while True:
            # Refresh the latest 15 docs
            global latest_15_data
            latest_15_data = list(collection.find().sort('_id', -1).limit(15))
            # Make sure we keep in correct order (oldest first)
            # latest_15_data.reverse()

            # Increment index for cycling
            # current_index["index"] = (current_index["index"] + 1) % len(latest_15_data)
            await asyncio.sleep(5)

    loop.run_until_complete(toggle())

# Start background task
threading.Thread(target=cycle_data_index, daemon=True).start()

@app.get("/farms")
async def get_farm_data():
    if not latest_15_data:
        # fallback in case background thread hasn't populated yet
        docs = list(collection.find().sort('_id', -1).limit(15))
        docs.reverse()
    else:
        docs = latest_15_data

    index = current_index["index"]
    if 0 <= index < len(docs):
        data = docs[index]
        return {
            "temperature": data.get("temperature"),
            "humidity": data.get("humidity"),
            "sunlight": data.get("sunlight")
        }
    else:
        return {
            "temperature": None,
            "humidity": None,
            "sunlight": None
        }