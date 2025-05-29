import requests
import json
import time
import sys

def test_thingsboard_connection(host, token):
    """Kiểm tra kết nối đến ThingsBoard và các API cơ bản"""
    
    base_url = f"https://{host}"
    telemetry_url = f"{base_url}/api/v1/{token}/telemetry"
    attributes_url = f"{base_url}/api/v1/{token}/attributes"
    
    print(f"Đang kiểm tra kết nối đến ThingsBoard tại {base_url}...")
    
    # Kiểm tra kết nối chung
    try:
        response = requests.get(f"{base_url}")
        print(f"Kết nối cơ bản: {'Thành công' if response.status_code == 200 else 'Thất bại'}")
    except Exception as e:
        print(f"Lỗi kết nối cơ bản: {str(e)}")
        print("Đảm bảo máy chủ ThingsBoard đang hoạt động và có thể truy cập từ mạng của bạn.")
        return False
    
    # Kiểm tra telemetry API
    test_data = {
        "test": True,
        "timestamp": int(time.time() * 1000)
    }
    
    try:
        print("\n--- Kiểm tra API Telemetry ---")
        print(f"Gửi dữ liệu thử nghiệm đến: {telemetry_url}")
        print(f"Dữ liệu: {json.dumps(test_data)}")
        
        response = requests.post(telemetry_url, json=test_data)
        success = response.status_code == 200
        
        print(f"Kết quả: {'Thành công' if success else 'Thất bại'}")
        print(f"Mã phản hồi: {response.status_code}")
        if not success:
            print(f"Phản hồi: {response.text}")
    except Exception as e:
        print(f"Lỗi khi gửi telemetry: {str(e)}")
        return False
    
    # Kiểm tra attributes API
    test_attrs = {
        "test_attribute": "test_value",
        "client_timestamp": int(time.time() * 1000)
    }
    
    try:
        print("\n--- Kiểm tra API Attributes ---")
        print(f"Gửi attributes thử nghiệm đến: {attributes_url}")
        print(f"Dữ liệu: {json.dumps(test_attrs)}")
        
        response = requests.post(attributes_url, json=test_attrs)
        success = response.status_code == 200
        
        print(f"Kết quả: {'Thành công' if success else 'Thất bại'}")
        print(f"Mã phản hồi: {response.status_code}")
        if not success:
            print(f"Phản hồi: {response.text}")
    except Exception as e:
        print(f"Lỗi khi gửi attributes: {str(e)}")
        return False
    
    print("\nKiểm tra hoàn tất!")
    return True

if __name__ == "__main__":
    if len(sys.argv) < 3:
        host = "app.coreiot.io"
        token = "lr3yyoh7v2b98pdmuj38"
        print(f"Sử dụng thông tin mặc định: Host={host}, Token={token}")
    else:
        host = sys.argv[1]
        token = sys.argv[2]
    
    test_thingsboard_connection(host, token)
