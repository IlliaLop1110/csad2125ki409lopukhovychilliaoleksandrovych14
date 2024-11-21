import serial
import time
<<<<<<< HEAD
import threading
import json
import os

CONFIG_FILE = 'config/game_config.json'

=======
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d

def setup_serial_port():
    try:
        port = input("Enter the serial port (e.g., /dev/ttyUSB0 or COM3): ")
<<<<<<< HEAD
        return serial.Serial(port, 9600, timeout=1)
=======
        return serial.Serial(port, 115200, timeout=1)
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
    except serial.SerialException as e:
        print(f"Error: {e}")
        exit(1)

<<<<<<< HEAD

def send_message(message, ser):
    try:
        ser.write((message + '\n').encode())
    except serial.SerialException as e:
        print(f"Error sending message: {e}")


=======
def send_message(message, ser):
    try:
        ser.write((message + '\n').encode())
        print(f"Sent: {message}")
    except serial.SerialException as e:
        print(f"Error sending message: {e}")

>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
def receive_message(ser):
    try:
        received = ser.readline().decode('utf-8', errors='ignore').strip()
        if received:
<<<<<<< HEAD
            print(received)
=======
            print(f"Received: {received}")
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
        return received
    except serial.SerialException as e:
        print(f"Error receiving message: {e}")
        return None

<<<<<<< HEAD

def receive_multiple_messages(ser, count):
    messages = []
    for _ in range(count):
        message = receive_message(ser)
        if message:
            messages.append(message)
    return messages


def user_input_thread(ser):
    global can_input
    while True:
        if can_input:
            user_message = input()
            if user_message.lower() == 'exit':
                print("Exiting...")
                global exit_program
                exit_program = True
                break
            elif user_message.lower().startswith('save'):
                save_game_config(user_message)
            elif user_message.lower().startswith('load'):
                file_path = input("Enter the path to the configuration file: ")
                load_game_config(file_path, ser)
            send_message(user_message, ser)
            can_input = False 


def monitor_incoming_messages(ser):
    global can_input
    global last_received_time
    while not exit_program:
        received = receive_message(ser)
        if received:
            last_received_time = time.time() 
            if not can_input:
                can_input = True  


def save_game_config(message):
    config = {
        "gameMode": 0,
        "playerChoices1": "Rock",
        "playerChoices2": "Paper",
        "playerChoices3": "Scissors"
    }

    try:
        params = message.split()
        if len(params) == 2 and params[1].isdigit():
            config["gameMode"] = int(params[1])

        with open(CONFIG_FILE, 'w') as f:
            json.dump(config, f, indent=4)
        print(f"Configuration saved to {CONFIG_FILE}")
    except Exception as e:
        print(f"Error saving configuration: {e}")

def load_game_config(file_path, ser):
    try:
        if os.path.exists(file_path):
            with open(file_path, 'r') as f:
                config = json.load(f)

                game_mode = config.get("gameMode", 0)
                player_choices = [
                    config.get("playerChoices1", "Rock"),
                    config.get("playerChoices2", "Paper"),
                    config.get("playerChoices3", "Scissors")
                ]

                print(f"Game Mode: {game_mode}")
                print(f"Player Choices: {player_choices}")

                json_message = {
                    "gameMode": game_mode,
                    "playerChoices1": player_choices[0],
                    "playerChoices2": player_choices[1],
                    "playerChoices3": player_choices[2]
                }

                json_str = json.dumps(json_message)
                print(json_str)

                send_message(json_str, ser)
        else:
            print("Configuration file not found. Please provide a valid path.")
    except Exception as e:
        print(f"Error loading configuration: {e}")



if __name__ == "__main__":
    ser = setup_serial_port()
    can_input = True
    exit_program = False
    last_received_time = time.time()

    threading.Thread(target=monitor_incoming_messages, args=(ser,), daemon=True).start()
    threading.Thread(target=user_input_thread, args=(ser,), daemon=True).start()

    try:
        while not exit_program:
            if time.time() - last_received_time >= 1 and can_input:
                pass
            else:
                time.sleep(0.1)
=======
if __name__ == "__main__":
    ser = setup_serial_port()
    try:
        while True:
            user_message = input("Message to server: ")
            if user_message.lower() == 'exit':
                print("Exiting...")
                break
            send_message(user_message, ser)
            receive_message(ser)
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
    except KeyboardInterrupt:
        print("Exit!")
    finally:
        if ser.is_open:
            print("Closing serial port...")
            ser.close()
