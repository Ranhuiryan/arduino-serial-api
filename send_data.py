import serial
import time
from flask import Flask, request, jsonify

# 初始化串口
arduino = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=.1)

# def write_to_arduino(data):
#     arduino.write(bytes(data, 'utf-8'))
#     time.sleep(0.05)
#     response = arduino.readline()
#     return response.decode('utf-8')

def convert_distance_to_pulse(distance):
    return distance / 0.5 * 800   # 0.5mm导程对应800个脉冲

def convert_pulse_to_distance(pulse):
    return pulse / 800 * 0.5

# 初始化Flask应用
app = Flask(__name__)

@app.route('/send_data', methods=['POST'])
def send_data():
    if request.is_json:
        req = request.get_json()
        speed = req.get("speed")    # 速度单位：mm/s
        acceleration = req.get("acceleration")
        distance = req.get("distance")     # 距离单位：mm
        
        if speed is not None and acceleration is not None and distance is not None:
            speed = convert_distance_to_pulse(speed)
            pulse = convert_distance_to_pulse(distance)
            data_to_send = f"{speed}, {acceleration}, {pulse}"
            # current_position = -999999
            arduino.write(bytes(data_to_send, 'utf-8'))
            time.sleep(0.05)
            # while int(arduino.readline().decode('utf-8').split(",")[0]) != current_position:
            #     response = arduino.readline().decode('utf-8')
            #     current_position = int(response.split(",")[0])
            #     current_speed = int(response.split(",")[1])
            #     time.sleep(1.05)

            # return jsonify({"status": "success", "position": current_position, "speed": current_speed}), 200
            return jsonify({"status": "success"}), 200
        else:
            return jsonify({"status": "error", "message": "Invalid data"}), 400
    else:
        return jsonify({"status": "error", "message": "Request must be JSON"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5500, debug=True)
