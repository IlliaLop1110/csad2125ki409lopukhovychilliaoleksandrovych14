import serial
import time

def setup_serial_port():
    port = input("Enter the serial port (e.g., /dev/ttyUSB0 or COM3): ")
    return serial.Serial(port, 115200, timeout=1)

def send_message(message, ser):
    ser.write((message + '\n').encode())

def receive_message(ser):
    received = ser.readline().decode().strip()
    if received:
        print(received)
    return received

if __name__ == "__main__":
    ser = setup_serial_port()
    try:
        while True:
            user_message = input("Message to server: ")
            send_message(user_message, ser)
            receive_message(ser)
    except KeyboardInterrupt:
        print("Exit!")
    finally:
        ser.close()
