@echo off

set /p PORT="Enter the serial port (e.g., COM3): "

if "%PORT%"=="" (
    echo No port specified. Exiting.
    exit /b 1
)

where platformio >nul 2>nul
if errorlevel 1 (
    echo PlatformIO is not installed. Please install it before running.
    exit /b 1
)

echo Installing Python dependencies...
pip install -r python_project\requirements.txt

echo Building firmware...
platformio run

echo Flashing ESP32...
platformio run --target upload --upload-port "%PORT%"

echo Running tests...
pytest python_project\tests\ --junitxml=python_project\tests\reports\results.xml

echo Running main.py...
python python_project\main.py

echo Completed successfully!
