# camera.py
from picamera2 import Picamera2
import time
import threading
import numpy as np

class VideoCamera(object):
    def __init__(self, flip=False, file_type=".jpg", photo_string="stream_photo"):
        self.picam2 = Picamera2()
        self.picam2.start()
        self.flip = flip
        self.file_type = file_type
        self.photo_string = photo_string

        time.sleep(2.0)  # Allow camera to warm up

    def __del__(self):
        self.picam2.stop()

    def flip_if_needed(self, frame):
        if self.flip:
            return np.flipud(np.fliplr(frame))
        return frame

    def get_frame(self):
        frame = self.picam2.capture_array()
        frame = self.flip_if_needed(frame)
        ret, jpeg = cv2.imencode(self.file_type, frame)
        if not ret:
            raise RuntimeError(f"Error: Unable to encode frame to {self.file_type} format.")
        return jpeg.tobytes()
