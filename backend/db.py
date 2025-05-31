from pymongo import MongoClient

# Connect to MongoDB server (make sure MongoDB is running locally)
client = MongoClient("mongodb://localhost:27017/")

# Create a database called "YoloFarm"
db = client["Yolo"]

# Create a collection called "YoloFarmData"
collection = db["YoloData"]

# (Optional) Clear old documents first if you want fresh start
collection.delete_many({})

# Create a list of fixed documents
data_list = [
    {"temperature": 100, "humidity": 10, "sunlight": 10},
    {"temperature": 200, "humidity": 20, "sunlight": 20},
    {"temperature": 300, "humidity": 30, "sunlight": 30},
    {"temperature": 400, "humidity": 40, "sunlight": 40},
    {"temperature": 500, "humidity": 50, "sunlight": 50},
    {"temperature": 600, "humidity": 60, "sunlight": 60},
    {"temperature": 700, "humidity": 70, "sunlight": 70},
    {"temperature": 800, "humidity": 80, "sunlight": 80},
    {"temperature": 900, "humidity": 90, "sunlight": 90},
    {"temperature": 1000, "humidity": 100, "sunlight": 100},
    {"temperature": 1100, "humidity": 110, "sunlight": 110},
    {"temperature": 1200, "humidity": 120, "sunlight": 120},
    {"temperature": 1300, "humidity": 130, "sunlight": 130},
    {"temperature": 1400, "humidity": 140, "sunlight": 140},
    {"temperature": 1500, "humidity": 150, "sunlight": 150}
]

# Insert all documents into the collection
result = collection.insert_many(data_list)

print(f"Inserted {len(result.inserted_ids)} fixed documents into YoloFarmData!")