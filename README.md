# Arduino Data Sender

This project is designed to facilitate communication between a Flask-based web API and an Arduino device. It allows users to send data such as speed, acceleration, and distance to the Arduino via a simple HTTP POST request. The data is then processed by the Arduino for various applications, such as controlling motors or logging data.

## Getting Started

### Prerequisites

- Python 3.8 or higher
- pip (Python package installer)

### Installation

1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Create a virtual environment:
   ```sh
   python3 -m venv .env
   ```
4. Activate the virtual environment:
   - On Windows:
     ```sh
     .env\Scripts\activate
     ```
   - On Unix or MacOS:
     ```sh
     source .env/bin/activate
     ```
5. Install the required dependencies:
   ```sh
   pip install -r requirements.txt
   ```

### Running the API

1. Ensure your Arduino device is connected to your computer.
2. Run the Flask application:
   ```sh
   python send_data.py
   ```
3. The API will be available at `http://127.0.0.1:5500/send_data`.

## Using the API

### Send Command

To send data to the Arduino, make a POST request to the API with a JSON payload containing the `speed`, `acceleration`, and `distance` values. For example:

```sh
curl -X POST http://127.0.0.1:5500/send_data -H "Content-Type: application/json" -d '{"speed": 800, "acceleration": 200, "distance": 300}'
```

## Checking Device Status

To check the current status of the Arduino device, you can make a GET request to the `/status` endpoint. This will return the current position and speed of the device.

```sh
curl http://127.0.0.1:5500/status
```

### Response

- The API returns a JSON response with the current `position` and `speed` of the Arduino device.
- Example response:
  ```json
  {
    "position": 100,
    "speed": 50
  }
  ```