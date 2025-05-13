from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/thingsboard-webhook', methods=['POST'])
def thingsboard_webhook():
    """
    Đây là endpoint mà ThingsBoard sẽ POST data tới.
    """
    data = request.get_json()  # JSON payload từ ThingsBoard
    # --- XỬ LÝ DATA TẠI ĐÂY ---
    # Ví dụ đơn giản: in ra console
    print("Received from ThingsBoard:", data)

    # Có thể lưu vào database, gọi service khác, v.v.

    # Trả về 200 OK để ThingsBoard biết là bạn đã nhận thành công
    return jsonify({"status": "received"}), 200

if __name__ == '__main__':
    # Chạy Flask ở cổng 5000, debug mode để tự reload khi sửa code
    app.run(host='0.0.0.0', port=5000, debug=True)
