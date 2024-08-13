import serial
import time
from flask import Flask, request, jsonify

# 初始化串口
arduino = serial.Serial(port='/dev/cu.usbserial-240', baudrate=9600, timeout=.1)

def convert_distance_to_pulse(distance):
    return distance / 0.5 * 800   # 0.5cm导程对应800个脉冲

def convert_pulse_to_distance(pulse):
    return pulse / 800 * 0.5

# 初始化Flask应用
app = Flask(__name__)

@app.route('/send_command', methods=['POST'])
def send_command():
    if request.is_json:
        req = request.get_json()
        speed = req.get("speed")    # 速度单位：cm/s
        acceleration = req.get("acceleration")
        distance = req.get("distance")     # 距离单位：cm
        
        if speed is not None and acceleration is not None and distance is not None:
            speed = convert_distance_to_pulse(speed)
            acceleration = convert_distance_to_pulse(acceleration)
            pulse = convert_distance_to_pulse(distance)
            data_to_send = f"{speed},{acceleration},{-pulse}"
            arduino.write(bytes(data_to_send, 'utf-8'))
            time.sleep(0.05)
            return jsonify({"status": "success"}), 200
        else:
            return jsonify({"status": "error", "message": "Invalid data"}), 400
    else:
        return jsonify({"status": "error", "message": "Request must be JSON"}), 400
    
@app.route('/status', methods=['GET'])
def status():
    time.sleep(0.05)
    data = arduino.readline().decode('utf-8').strip()
    if data == "":
        return jsonify({"position": 0, "speed": 0}), 200
    else:
        position, speed = data.split(',')
        return jsonify({"position": convert_pulse_to_distance(-int(position)), "speed": convert_pulse_to_distance(float(speed))}), 200
        # return jsonify({"position": data, "speed": 0}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5500, debug=True)
