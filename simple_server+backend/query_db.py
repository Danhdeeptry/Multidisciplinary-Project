import sqlite3
conn = sqlite3.connect('data/sensor_data.db')
cursor = conn.cursor()
cursor.execute('SELECT * FROM telemetry_data ORDER BY id DESC LIMIT 10')
for row in cursor.fetchall():
    print(row)
conn.close()
