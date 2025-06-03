import uvicorn
from pyngrok import ngrok
import os
import sys

def run_app():
    # Lấy token Ngrok từ biến môi trường hoặc yêu cầu người dùng nhập
    ngrok_token = os.environ.get("NGROK_TOKEN")
    if not ngrok_token:
        print("Bạn cần cung cấp Ngrok token.")
        print("Đăng ký tại: https://dashboard.ngrok.com/signup")
        ngrok_token = input("Nhập Ngrok token của bạn: ").strip()
        if not ngrok_token:
            print("Không có token, thoát chương trình.")
            sys.exit(1)
    
    # Thêm token vào cấu hình Ngrok
    ngrok.set_auth_token(ngrok_token)
    
    # Khởi tạo tunnel Ngrok
    port = 8000
    public_url = ngrok.connect(port).public_url
    print(f"Server khả dụng tại URL công khai: {public_url}")
    print(f"API sẽ khả dụng tại URL: {public_url}/thingsboard-data")
    print("Bạn có thể sử dụng URL này để cấu hình trong ThingsBoard")
    
    # Khởi động server FastAPI
    uvicorn.run("main:app", host="0.0.0.0", port=port)

if __name__ == "__main__":
    run_app()
