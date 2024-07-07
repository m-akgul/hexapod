# -*- coding: utf-8 -*-
#!/usr/bin/env python3
##################################
# MLX90640 Thermal Camera w Raspberry Pi
##################################
import time, board, busio
import numpy as np
import adafruit_mlx90640
import cv2
from scipy import ndimage

class pithermalcam:
    i2c = None
    mlx = None
    _temp_min = None
    _temp_max = None
    _raw_image = None
    _image = None

    def __init__(self):
        self._setup_therm_cam()
        self._t0 = time.time()
        self.update_image_frame()
    
    def getMax(self):
        return self._temp_max

    def _setup_therm_cam(self):
        """Initialize the thermal camera"""
        self.i2c = busio.I2C(board.SCL, board.SDA, frequency=800000)
        self.mlx = adafruit_mlx90640.MLX90640(self.i2c)
        self.mlx.refresh_rate = adafruit_mlx90640.RefreshRate.REFRESH_4_HZ
        time.sleep(0.1)

    def _pull_raw_image(self):
        """Get one pull of the raw image data"""
        self._raw_image = np.zeros((24*32,))
        try:
            self.mlx.getFrame(self._raw_image)
            self._temp_min = np.min(self._raw_image)
            self._temp_max = np.max(self._raw_image)
            self._raw_image = self._temps_to_rescaled_uints(self._raw_image, self._temp_min, self._temp_max)
        except ValueError:
            print("Math error; continuing...")
            self._raw_image = np.zeros((24*32,))
        except OSError:
            print("IO Error; continuing...")
            self._raw_image = np.zeros((24*32,))

    def _process_raw_image(self):
        """Process the raw temp data to a colored image"""
        self._image = ndimage.zoom(self._raw_image, 10)  # interpolate with scipy
        self._image = cv2.applyColorMap(self._image, cv2.COLORMAP_JET)
        self._image = cv2.resize(self._image, (800, 600), interpolation=cv2.INTER_CUBIC)
        self._image = cv2.flip(self._image, 1)

    def _add_image_text(self):
        """Set image text content"""
        text = f'Tmin={self._temp_min:+.1f}C - Tmax={self._temp_max:+.1f}C'
        cv2.putText(self._image, text, (30, 22), cv2.FONT_HERSHEY_SIMPLEX, .7, (255, 255, 255), 2)
        self._t0 = time.time()

    def update_image_frame(self):
        """Pull raw temperature data, process it to an image, and update image text"""
        self._pull_raw_image()
        self._process_raw_image()
        self._add_image_text()
        return self._image

    def display_camera_onscreen(self):
        """Display the camera live to the display"""
        while True:
            try:
                self.update_image_frame()
                cv2.imshow('Thermal Image', self._image)
                if cv2.waitKey(1) & 0xFF == 27:  # Exit on ESC key
                    cv2.destroyAllWindows()
                    break
            except RuntimeError as e:
                if e.message == 'Too many retries':
                    print("Too many retries error caught, potential I2C baudrate issue: continuing...")
                    continue
                raise

    def _temps_to_rescaled_uints(self, f, Tmin, Tmax):
        """Function to convert temperatures to pixels on image"""
        f = np.nan_to_num(f)
        norm = np.uint8((f - Tmin) * 255 / (Tmax - Tmin))
        norm.shape = (24, 32)
        return norm

if __name__ == "__main__":
    thermcam = pithermalcam()
    thermcam.display_camera_onscreen()
