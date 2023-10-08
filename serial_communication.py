import time

def send_to_arduino(input_str, ser):
    ser.write((input_str + "\n").encode())
    time.sleep(0.1)

def send_to_arduino_slave(input_str, ser):
    ser.write(input_str.encode())
    time.sleep(0.1)

def read_from_arduino(ser):
    response = ser.readline().decode().strip()
    return response

