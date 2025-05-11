from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pymongo import MongoClient
from bson.objectid import ObjectId

app = FastAPI()

# Allow frontend (React/Vue) to access this API
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # You can change to your frontend URL for security
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Connect to MongoDB
client = MongoClient("mongodb://localhost:27017/")
db = client["YoloFarm"]
collection = db["YoloFarmData"]

@app.get("/farms")
async def get_farm_data():
    # Find the latest document (sort by _id in descending order)
    latest_data = collection.find_one(sort=[('_id', -1)])
    
    if latest_data:
        return {
            "temperature": latest_data.get("temperature"),
            "humidity": latest_data.get("humidity")
        }
    else:
        return {
            "temperature": None,
            "humidity": None
        }