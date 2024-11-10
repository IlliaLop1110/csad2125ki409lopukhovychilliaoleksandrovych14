import pytest
from unittest.mock import patch, MagicMock
import serial
import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from main import send_message, receive_message


def test_send_message():
    mock_serial = MagicMock(spec=serial.Serial)
    send_message("Hello", mock_serial)
    mock_serial.write.assert_called_with(b"Hello\n")

def test_receive_message():
    mock_serial = MagicMock(spec=serial.Serial)
    mock_serial.readline.return_value = b"Test Message\n"
    result = receive_message(mock_serial)
    assert result == "Test Message"

def test_receive_empty_message():
    mock_serial = MagicMock(spec=serial.Serial)
    mock_serial.readline.return_value = b"\n"
    result = receive_message(mock_serial)
    assert result == ""

@patch('builtins.input', return_value='COM3')  # Мокінг input для тестів
def test_serial_port(mock_input):
    mock_serial = MagicMock(spec=serial.Serial)
    
    # Мокінг атрибута portstr
    mock_serial.portstr = 'COM3'  # Визначаємо значення атрибута portstr
    
    port = 'COM3'
    ser = mock_serial
    
    # Тестуємо значення
    assert ser.portstr == port