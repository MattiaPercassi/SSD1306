Library for connecting SSD1306 OLED display in the verison 128x32 to Raspberry Pi through I2C bus.
This small driver uses the existing pigpio library for writing bytes through gpio.

Current functionality
- Display init and close routines
- Standard commands: all pixels ON, resume RAM display, turn OFF, reset cursor to start (page mode), invert display
- Load images from 1bpp bitmap files (.bmp) both 128x32 and 32x128 formats
- Write image to screen
- empty RAM: reset RAM to 0 value
- fill all RAM with the same byte
- write string (currently only capital letters and numbers with 6 pixel height custom font)

