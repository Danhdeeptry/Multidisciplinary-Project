from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pymongo import MongoClient
from bson.json_util import dumps
from bson.objectid import ObjectId
import asyncio
import threading

app = FastAPI()

# Allow CORS (so frontend like React or JS can access)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # You can set this to your frontend domain
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# MongoDB connection setup
client = MongoClient("mongodb://localhost:27017")
db = client["YoloFarms"]
collection = db["YoloFarmsData"]

current_index = {"index": 0}
latest_data = []  # Cache of latest 15 documents

def cycle_data_index():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    
    async def toggle():
        while True:
            # Refresh the latest 15 docs
            global latest_data
            latest_data = list(collection.find().sort('_id', -1).limit(15))
            # Make sure we keep in correct order (oldest first)
            # latest_data.reverse()

            # Increment index for cycling
            # current_index["index"] = (current_index["index"] + 1) % len(latest_data)
            await asyncio.sleep(0.1)

    loop.run_until_complete(toggle())

# Start background task
threading.Thread(target=cycle_data_index, daemon=True).start()


@app.get("/YoloFarms")
# def get_latest_data():
#     # latest = collection.find_one(sort=[("_id", -1)])
#     # if latest:
#     #     return {
#     #         "temperature": latest.get("temperature"),
#     #         "humidity": latest.get("humidity")
#     #     }
#     # return {"error": "No data found"}

async def get_farm_data():
    if not latest_data:
        # fallback in case background thread hasn't populated yet
        docs = list(collection.find().sort('_id', -1).limit(15))
        docs.reverse()
    else:
        docs = latest_data

    index = current_index["index"]
    if 0 <= index < len(docs):
        data = docs[index]
        return {
            "temperature": data.get("temperature"),
            "humidity": data.get("humidity"),
        }
    else:
        return {
            "temperature": None,
            "humidity": None,
        }