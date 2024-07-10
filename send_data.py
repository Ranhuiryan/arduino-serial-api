import serial
import time
from flask import Flask, request, jsonify

# 初始化串口
arduino = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=.1)

def write_to_arduino(data):
    arduino.write(bytes(data, 'utf-8'))
    time.sleep(0.05)
    response = arduino.readline()
    return response.decode('utf-8')

# 初始化Flask应用
app = Flask(__name__)

@app.route('/send_data', methods=['POST'])
def send_data():
    if request.is_json:
        req = request.get_json()
        speed = req.get("speed")
        acceleration = req.get("acceleration")
        distance = req.get("distance")
        
        if speed is not None and acceleration is not None and distance is not None:
            data_to_send = f"{speed}, {acceleration}, {distance}"
            response = write_to_arduino(data_to_send)
            return jsonify({"status": "success", "response": data_to_send}), 200
        else:
            return jsonify({"status": "error", "message": "Invalid data"}), 400
    else:
        return jsonify({"status": "error", "message": "Request must be JSON"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5500, debug=True)
