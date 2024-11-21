import serial
import time
import threading
import json
import os

CONFIG_FILE = 'config/game_config.json'


def setup_serial_port():
    """!
    @brief Sets up the serial port for communication.
    @details Prompts the user to input the serial port name and initializes a serial connection.
    @return A configured serial.Serial object.
    """
    try:
        port = input("Enter the serial port (e.g., /dev/ttyUSB0 or COM3): ")
        return serial.Serial(port, 9600, timeout=1)
    except serial.SerialException as e:
        print(f"Error: {e}")
        exit(1)


def send_message(message, ser):
    """!
    @brief Sends a message over the serial port.
    @param message The message string to send.
    @param ser The serial.Serial object for communication.
    """
    try:
        ser.write((message + '\n').encode())
    except serial.SerialException as e:
        print(f"Error sending message: {e}")


def receive_message(ser):
    """!
    @brief Receives a message from the serial port.
    @param ser The serial.Serial object for communication.
    @return The received message as a string, or None if no message is received.
    """
    try:
        received = ser.readline().decode('utf-8', errors='ignore').strip()
        if received:
            print(received)
        return received
    except serial.SerialException as e:
        print(f"Error receiving message: {e}")
        return None


def receive_multiple_messages(ser, count):
    """!
    @brief Receives multiple messages from the serial port.
    @param ser The serial.Serial object for communication.
    @param count The number of messages to receive.
    @return A list of received messages.
    """
    messages = []
    for _ in range(count):
        message = receive_message(ser)
        if message:
            messages.append(message)
    return messages


def user_input_thread(ser):
    """!
    @brief Handles user input in a separate thread.
    @details Reads commands from the user, processes them, and sends them to the serial port.
    @param ser The serial.Serial object for communication.
    """
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
    """!
    @brief Monitors incoming messages from the serial port in a separate thread.
    @param ser The serial.Serial object for communication.
    """
    global can_input
    global last_received_time
    while not exit_program:
        received = receive_message(ser)
        if received:
            last_received_time = time.time() 
            if not can_input:
                can_input = True  


def save_game_config(message):
    """!
    @brief Saves game configuration to a file.
    @param message The user input containing game mode details.
    """
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
    """!
    @brief Loads game configuration from a file and sends it over the serial port.
    @param file_path The path to the configuration file.
    @param ser The serial.Serial object for communication.
    """
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
    """!
    @brief Main function to initialize serial communication and handle threads.
    """
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
    except KeyboardInterrupt:
        print("Exit!")
    finally:
        if ser.is_open:
            print("Closing serial port...")
            ser.close()
