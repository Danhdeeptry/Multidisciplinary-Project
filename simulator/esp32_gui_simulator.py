import tkinter as tk
from tkinter import ttk
import threading
import time
import random
import requests
import json
from datetime import datetime

# Thay đổi thông tin theo cấu hình thực tế của bạn
THINGSBOARD_HOST = "app.coreiot.io"
ACCESS_TOKEN = "lr3yyoh7v2b98pdmuj38"

# API endpoint của ThingsBoard
TELEMETRY_ENDPOINT = f"https://{THINGSBOARD_HOST}/api/v1/{ACCESS_TOKEN}/telemetry"
ATTRIBUTES_ENDPOINT = f"https://{THINGSBOARD_HOST}/api/v1/{ACCESS_TOKEN}/attributes"
RPC_ENDPOINT = f"https://{THINGSBOARD_HOST}/api/v1/{ACCESS_TOKEN}/rpc"

class ESP32Simulator:
    def __init__(self, root):
        self.root = root
        self.root.title("ESP32 IoT Simulator")
        self.root.geometry("600x500")
        self.root.resizable(True, True)
        
        # Trạng thái thiết bị
        self.temperature = 28.0
        self.humidity = 65.0
        self.led_state = False
        self.is_running = False
        self.is_connected = False
        
        # Biến theo dõi luồng
        self.simulation_thread = None
        
        # Tạo giao diện
        self.create_widgets()
        
    def create_widgets(self):
        # Frame chính
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Thông tin kết nối
        conn_frame = ttk.LabelFrame(main_frame, text="Thông tin kết nối", padding="10")
        conn_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(conn_frame, text="ThingsBoard Host:").grid(row=0, column=0, sticky=tk.W)
        self.host_entry = ttk.Entry(conn_frame, width=30)
        self.host_entry.grid(row=0, column=1, padx=5, pady=2, sticky=tk.W)
        self.host_entry.insert(0, THINGSBOARD_HOST)
        
        ttk.Label(conn_frame, text="Access Token:").grid(row=1, column=0, sticky=tk.W)
        self.token_entry = ttk.Entry(conn_frame, width=30)
        self.token_entry.grid(row=1, column=1, padx=5, pady=2, sticky=tk.W)
        self.token_entry.insert(0, ACCESS_TOKEN)
        
        self.conn_status = ttk.Label(conn_frame, text="Trạng thái: Chưa kết nối", foreground="red")
        self.conn_status.grid(row=0, column=2, rowspan=2, padx=20)
        
        self.connect_btn = ttk.Button(conn_frame, text="Kết nối", command=self.toggle_connection)
        self.connect_btn.grid(row=0, column=3, rowspan=2, padx=5, pady=5)
        
        # Thông số cảm biến
        sensor_frame = ttk.LabelFrame(main_frame, text="Thông số cảm biến", padding="10")
        sensor_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(sensor_frame, text="Nhiệt độ (°C):").grid(row=0, column=0, sticky=tk.W)
        self.temp_var = tk.DoubleVar(value=self.temperature)
        self.temp_slider = ttk.Scale(sensor_frame, from_=15.0, to=40.0, orient=tk.HORIZONTAL, 
                                 variable=self.temp_var, length=200)
        self.temp_slider.grid(row=0, column=1, padx=5, pady=2)
        self.temp_label = ttk.Label(sensor_frame, text=f"{self.temperature:.2f} °C")
        self.temp_label.grid(row=0, column=2, padx=5)
        
        ttk.Label(sensor_frame, text="Độ ẩm (%):").grid(row=1, column=0, sticky=tk.W)
        self.humid_var = tk.DoubleVar(value=self.humidity)
        self.humid_slider = ttk.Scale(sensor_frame, from_=30.0, to=90.0, orient=tk.HORIZONTAL, 
                                   variable=self.humid_var, length=200)
        self.humid_slider.grid(row=1, column=1, padx=5, pady=2)
        self.humid_label = ttk.Label(sensor_frame, text=f"{self.humidity:.2f} %")
        self.humid_label.grid(row=1, column=2, padx=5)
        
        # Trạng thái LED
        led_frame = ttk.LabelFrame(main_frame, text="Điều khiển LED", padding="10")
        led_frame.pack(fill=tk.X, pady=5)
        
        self.led_var = tk.BooleanVar(value=self.led_state)
        self.led_check = ttk.Checkbutton(led_frame, text="LED", variable=self.led_var, 
                                     command=self.toggle_led)
        self.led_check.grid(row=0, column=0, padx=5, pady=2)
        
        self.led_indicator = tk.Canvas(led_frame, width=30, height=30, bg="white")
        self.led_indicator.grid(row=0, column=1, padx=5, pady=2)
        self.led_circle = self.led_indicator.create_oval(5, 5, 25, 25, fill="gray")
        
        # Nút điều khiển
        control_frame = ttk.Frame(main_frame, padding="10")
        control_frame.pack(fill=tk.X, pady=5)
        
        self.start_btn = ttk.Button(control_frame, text="Bắt đầu mô phỏng", command=self.start_simulation)
        self.start_btn.pack(side=tk.LEFT, padx=5)
        
        self.stop_btn = ttk.Button(control_frame, text="Dừng mô phỏng", command=self.stop_simulation, state=tk.DISABLED)
        self.stop_btn.pack(side=tk.LEFT, padx=5)
        
        # Terminal để hiển thị log
        log_frame = ttk.LabelFrame(main_frame, text="Terminal Log", padding="10")
        log_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        self.log_text = tk.Text(log_frame, height=10, wrap=tk.WORD)
        self.log_text.pack(fill=tk.BOTH, expand=True)
        
        # Thanh cuộn cho terminal
        scrollbar = ttk.Scrollbar(self.log_text, command=self.log_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.log_text.config(yscrollcommand=scrollbar.set)
        
        # Đăng ký cập nhật giá trị từ sliders
        self.temp_var.trace("w", self.update_temp_label)
        self.humid_var.trace("w", self.update_humid_label)
        
        # Cập nhật thông số định kỳ
        self.root.after(100, self.update_ui)
        
    def log(self, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.log_text.insert(tk.END, f"[{timestamp}] {message}\n")
        self.log_text.see(tk.END)
        
    def toggle_connection(self):
        if not self.is_connected:
            self.is_connected = True
            self.conn_status.config(text="Trạng thái: Đã kết nối", foreground="green")
            self.connect_btn.config(text="Ngắt kết nối")
            self.log("Kết nối thành công đến ThingsBoard")
        else:
            self.is_connected = False
            self.conn_status.config(text="Trạng thái: Chưa kết nối", foreground="red")
            self.connect_btn.config(text="Kết nối")
            if self.is_running:
                self.stop_simulation()
            self.log("Đã ngắt kết nối khỏi ThingsBoard")
    
    def update_temp_label(self, *args):
        self.temperature = self.temp_var.get()
        self.temp_label.config(text=f"{self.temperature:.2f} °C")
        
    def update_humid_label(self, *args):
        self.humidity = self.humid_var.get()
        self.humid_label.config(text=f"{self.humidity:.2f} %")
        
    def toggle_led(self):
        self.led_state = self.led_var.get()
        if self.led_state:
            self.led_indicator.itemconfig(self.led_circle, fill="green")
            self.log("LED bật")
        else:
            self.led_indicator.itemconfig(self.led_circle, fill="gray")
            self.log("LED tắt")
            
    def start_simulation(self):
        if not self.is_connected:
            self.log("Vui lòng kết nối trước khi bắt đầu mô phỏng")
            return
            
        self.is_running = True
        self.start_btn.config(state=tk.DISABLED)
        self.stop_btn.config(state=tk.NORMAL)
        self.log("Bắt đầu mô phỏng ESP32")
        
        # Khởi tạo và bắt đầu một luồng mới
        self.simulation_thread = threading.Thread(target=self.simulation_loop)
        self.simulation_thread.daemon = True
        self.simulation_thread.start()
        
    def stop_simulation(self):
        self.is_running = False
        self.start_btn.config(state=tk.NORMAL)
        self.stop_btn.config(state=tk.DISABLED)
        self.log("Dừng mô phỏng ESP32")
        
    def simulation_loop(self):
        # Thời gian cuối cùng gửi dữ liệu
        last_telemetry_time = 0
        last_attributes_time = 0
        
        while self.is_running:
            current_time = time.time()
            
            # Gửi telemetry mỗi 2 giây
            if current_time - last_telemetry_time >= 2:
                last_telemetry_time = current_time
                self.send_telemetry()
                
                # Thêm dao động ngẫu nhiên
                if not self.temp_slider.instate(['pressed']):  # Chỉ thêm dao động khi người dùng không điều chỉnh
                    new_temp = self.temperature + random.randint(-100, 100) / 100.0
                    new_temp = max(15.0, min(new_temp, 40.0))
                    self.temp_var.set(new_temp)
                
                if not self.humid_slider.instate(['pressed']):
                    new_humid = self.humidity + random.randint(-50, 50) / 100.0
                    new_humid = max(30.0, min(new_humid, 90.0))
                    self.humid_var.set(new_humid)
            
            # Gửi attributes mỗi 5 giây
            if current_time - last_attributes_time >= 5:
                last_attributes_time = current_time
                self.send_attributes()
                
            # Tạm dừng một chút để không tốn CPU
            time.sleep(0.1)
    
    def send_telemetry(self):
        if not self.is_connected:
            return
            
        telemetry = {
            "temperature": self.temperature,
            "humidity": self.humidity,
            "timestamp": int(time.time() * 1000)
        }
        
        try:
            host = self.host_entry.get()
            token = self.token_entry.get()
            endpoint = f"https://{host}/api/v1/{token}/telemetry"
            
            # Trong môi trường mô phỏng, chỉ log ra màn hình thay vì gửi HTTP thực sự
            # Trong thực tế, bạn có thể sử dụng dòng comment bên dưới
            # response = requests.post(endpoint, json=telemetry)
            
            self.log(f"Gửi telemetry: {json.dumps(telemetry)}")
        except Exception as e:
            self.log(f"Lỗi khi gửi telemetry: {str(e)}")
            
    def send_attributes(self):
        if not self.is_connected:
            return
            
        attributes = {
            "rssi": random.randint(-90, -30),
            "channel": random.randint(1, 13),
            "bssid": "AA:BB:CC:DD:EE:FF",
            "localIp": "192.168.1." + str(random.randint(2, 254)),
            "ssid": "P4.1111",
            "ledState": self.led_state
        }
        
        try:
            host = self.host_entry.get()
            token = self.token_entry.get()
            endpoint = f"https://{host}/api/v1/{token}/attributes"
            
            # Trong môi trường mô phỏng, chỉ log ra màn hình thay vì gửi HTTP thực sự
            self.log(f"Gửi attributes: {json.dumps(attributes)}")
        except Exception as e:
            self.log(f"Lỗi khi gửi attributes: {str(e)}")
    
    def update_ui(self):
        # Không cần làm gì đặc biệt ở đây, chỉ đảm bảo UI được cập nhật
        self.root.after(100, self.update_ui)

if __name__ == "__main__":
    root = tk.Tk()
    app = ESP32Simulator(root)
    root.mainloop()
