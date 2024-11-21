@echo off

<<<<<<< HEAD
if "%PORT%"=="COM9" (
    if "%~1"=="" (
        echo No port specified. Exiting.
        exit /b 1
    ) else (
        set PORT=%~1
    )
=======
set /p PORT="Enter the serial port (e.g., COM3): "

if "%PORT%"=="" (
    echo No port specified. Exiting.
    exit /b 1
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
)

where platformio >nul 2>nul
if errorlevel 1 (
    echo PlatformIO is not installed. Installing PlatformIO...
    pip install platformio
    if errorlevel 1 (
        echo Failed to install PlatformIO. Please install it manually.
        exit /b 1
    )
)

echo Installing Python dependencies...
pip install -r python_project\requirements.txt

<<<<<<< HEAD
echo Installing PlatformIO libraries...
platformio lib install "ArduinoJson"
if errorlevel 1 (
    echo Failed to install ArduinoJson library. Exiting.
    exit /b 1
)

=======
>>>>>>> a02f3e4adcc3335a97dc2c1d5a2d9201f39a815d
echo Building firmware...
platformio run

echo Flashing ESP32...
platformio run --target upload --upload-port "%PORT%"

echo Running tests...
pytest python_project\tests\ --junitxml=python_project\tests\reports\results.xml

echo Running main.py...
python python_project\main.py

echo Completed successfully!
