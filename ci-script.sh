#!/bin/bash

read -p "Enter the serial port (e.g., /dev/ttyUSB0 or COM3): " PORT

if [ -z "$PORT" ]; then
    echo "No port specified. Exiting."
    exit 1
fi

if ! command -v platformio &> /dev/null
then
    echo "PlatformIO is not installed. Please install it before running."
    exit 1
fi

echo "Installing Python dependencies..."
pip install -r python_project/requirements.txt

echo "Building firmware..."
platformio run

echo "Flashing ESP32..."
platformio run --target upload --upload-port "$PORT"

echo "Running tests..."
pytest python_project/tests/ --junitxml=python_project/tests/reports/results.xml

echo "Running main.py..."
python3 python_project/main.py

echo "Completed successfully!"
