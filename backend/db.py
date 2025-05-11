from pymongo import MongoClient

# Connect to MongoDB server (make sure MongoDB is running locally)
client = MongoClient("mongodb://localhost:27017/")

# Create a database called "YoloFarm"
db = client["YoloFarm"]

# Create a collection called "YoloFarmData"
collection = db["YoloFarmData"]

# (Optional) Clear old documents first if you want fresh start
collection.delete_many({})

# Create a list of fixed documents
data_list = [
    {"temperature": 25.5, "humidity": 60},
    {"temperature": 26.0, "humidity": 58},
    {"temperature": 24.8, "humidity": 65},
    {"temperature": 27.2, "humidity": 62},
    {"temperature": 23.5, "humidity": 55},
    {"temperature": 28.1, "humidity": 70},
    {"temperature": 25.0, "humidity": 68},
    {"temperature": 26.7, "humidity": 61},
    {"temperature": 24.3, "humidity": 59},
    {"temperature": 27.5, "humidity": 63},
    {"temperature": 26.2, "humidity": 66},
    {"temperature": 25.8, "humidity": 64},
    {"temperature": 24.6, "humidity": 60},
    {"temperature": 28.3, "humidity": 71},
    {"temperature": 23.9, "humidity": 57}
]

# Insert all documents into the collection
result = collection.insert_many(data_list)

print(f"Inserted {len(result.inserted_ids)} fixed documents into YoloFarmData!")