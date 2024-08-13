import serial
import time

# 初始化串口
arduino = serial.Serial(port='/dev/cu.usbserial-240', baudrate=9600, timeout=.1)

while True:
    data = arduino.readline().decode('utf-8').strip()
    if data != "":
        print(data)
    time.sleep(1)