#!/tez/env python
from pi_therm_cam import pithermalcam
from flask import Flask, render_template, Response, request, jsonify
from flask_cors import CORS
import threading
import serial
import time, socket, logging, traceback
import cv2
from camera import VideoCamera
import numpy as np
from gpiozero import DistanceSensor
import os

# Initialize the camera object
your_camera = VideoCamera(flip=False)  # Adjust parameters if needed

sensor = DistanceSensor(echo=27, trigger=17)
distance = 0

# Set up Logger
logging.basicConfig(filename='pithermcam.log', filemode='a',
                    format='%(asctime)s %(levelname)-8s [%(filename)s:%(name)s:%(lineno)d] %(message)s',
                    level=logging.WARNING, datefmt='%d-%b-%y %H:%M:%S')
logger = logging.getLogger(__name__)

# initialize the output frames and locks used to ensure thread-safe exchanges of the output frames (useful when multiple browsers/tabs are viewing the stream)
outputFrame = None
thermcam = None
lock = threading.Lock()

camera_outputFrame = None
camera_lock = threading.Lock()

# App Globals
app = Flask(__name__)
CORS(app)

serial_port = '/dev/ttyUSB0'
# serial_port = 'COM3'

baud_rate = 9600
ser = serial.Serial(serial_port, baud_rate, timeout=1)
time.sleep(2)

@app.route('/')
def index():
    return render_template('index.html')

def gen():
    global camera_outputFrame, camera_lock
    while True:
        with camera_lock:
            if camera_outputFrame is None:
                continue
            (flag, encodedImage) = cv2.imencode(".jpg", camera_outputFrame)
            if not flag:
                continue
        yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + bytearray(encodedImage) + b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/thermal_video_feed')
def thermal_video_feed():
    # return the response generated along with the specific media type (mime type)
    return Response(generate(), mimetype="multipart/x-mixed-replace; boundary=frame")

def pull_images():
    global thermcam, outputFrame
    # loop over frames from the video stream
    while thermcam is not None:
        current_frame = None
        try:
            current_frame = thermcam.update_image_frame()
        except Exception:
            print("Too many retries error caught; continuing...")
            logger.info(traceback.format_exc())

        # If we have a frame, acquire the lock, set the output frame, and release the lock
        if current_frame is not None:
            with lock:
                outputFrame = current_frame.copy()

def generate():
    # grab global references to the output frame and lock variables
    global outputFrame, lock
    # loop over frames from the output stream
    while True:
        # wait until the lock is acquired
        with lock:
            # check if the output frame is available, otherwise skip the iteration of the loop
            if outputFrame is None:
                continue
            # encode the frame in JPEG format
            (flag, encodedImage) = cv2.imencode(".jpg", outputFrame)
            # ensure the frame was successfully encoded
            if not flag:
                continue
        # yield the output frame in the byte format
        yield (b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + bytearray(encodedImage) + b'\r\n')

def pull_camera_images():
    global camera_outputFrame, camera_lock, your_camera
    while True:
        frame = your_camera.get_frame()
        with camera_lock:
            camera_outputFrame = cv2.imdecode(np.frombuffer(frame, np.uint8), cv2.IMREAD_COLOR)

@app.route('/button_action', methods=['POST'])
def button_action():
    action = request.json.get('action')
    actions = {
        'manual': 'manual mod on',
        'automatic': 7,
        'stop': 0,
        'forward': 1,
        'backward': 2,
        'right-forward': 3,
        'right-backward': 4,
        'left-forward': 5,
        'left-backward': 6
    }
    print(actions.get(action, 'unknown action'))
    value = actions.get(action)
    if value is not None:
        if value == 7:
            if distance < 25:
                ser.write((str(3) + '\n').encode())
                print(f"Sent: 3")
                return jsonify({"status": "success", "sent_value": 3}), 200
            else:
                ser.write((str(1) + '\n').encode())
                print(f"Sent: 1")
                return jsonify({"status": "success", "sent_value": 1}), 200
        else:
            try:
                # Send the value to the Arduino
                ser.write((str(value) + '\n').encode())
                print(f"Sent: {value}")
                return jsonify({"status": "success", "sent_value": value}), 200
            except Exception as e:
                print(f"Error: {e}")
                return jsonify({"status": "error", "message": str(e)}), 500
    else:
        return jsonify({"status": "error", "message": "Invalid command"}), 400

def update_distance():
    global distance
    while True:
        distance = round(sensor.distance * 100, 2)
        time.sleep(1)

@app.route('/get_max_value', methods=['GET'])
def get_max_value():
    max_value = thermcam.getMax()
    return jsonify(max=max_value)

def get_ip_address():
    """Find the current IP address of the device"""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip_address = s.getsockname()[0]
    s.close()
    return ip_address

def start_server():
    global thermcam
    # initialize the video stream and allow the camera sensor to warmup
    thermcam = pithermalcam()
    time.sleep(0.1)

    # start a thread that will perform motion detection
    t = threading.Thread(target=pull_images)
    t.daemon = True
    t.start()

    # start a thread that will pull camera frames
    camera_thread = threading.Thread(target=pull_camera_images)
    camera_thread.daemon = True
    camera_thread.start()

    distance_thread = threading.Thread(target=update_distance)
    distance_thread.daemon = True
    distance_thread.start()

    ip = get_ip_address()
    port = 8000

    print(f'Server can be found at {ip}:{port}')

    # start the flask app
    app.run(host=ip, port=port, debug=False, threaded=True, use_reloader=False)

if __name__ == '__main__':
    start_server()
