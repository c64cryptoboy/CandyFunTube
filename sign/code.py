"""
RASTER EYES for Adafruit Matrix Portal: animated spooky eyes.
"""

# pylint: disable=import-error
import math
import random
import time
import board
import digitalio
import displayio
from adafruit_imageload import load
from adafruit_matrixportal.matrix import Matrix

matrix = Matrix(bit_depth=6)
display = matrix.display
frame_length = 0.4

distanced, palette_d = load('bmp/distanced.bmp', bitmap=displayio.Bitmap, palette=displayio.Palette)
candy1, palette1 = load('bmp/candy1.bmp', bitmap=displayio.Bitmap, palette=displayio.Palette)
candy2, palette2 = load('bmp/candy2.bmp', bitmap=displayio.Bitmap, palette=displayio.Palette)
candies = [candy1, candy2]

tilegrid1 = displayio.TileGrid(candy1, pixel_shader=palette1,
                                width = 1, height=1,
                                tile_width=64, tile_height=32,
                                default_tile=0)
tilegrid2 = displayio.TileGrid(candy2, pixel_shader=palette2,
                                width = 1, height=1,
                                tile_width=64, tile_height=32,
                                default_tile=0)

default_grid = displayio.TileGrid(distanced, pixel_shader=palette_d,
                                width = 1, height=1,
                                tile_width=64, tile_height=32,
                                default_tile=0)
default_group = displayio.Group()
default_group.append(default_grid)

candy1_group = displayio.Group()
candy1_group.append(tilegrid1)

candy2_group = displayio.Group()
candy2_group.append(tilegrid2)

groups = [candy1_group, candy2_group]

busypin = digitalio.DigitalInOut(board.A1)
busypin.pull = digitalio.Pull.UP

# MAIN LOOP ----------------------------------------------------------------

last_time = time.monotonic()
i = 0

while True:
    busy = busypin.value
    if not busy:
        now = time.monotonic()
        if now - last_time > frame_length:
            last_time = now  # Start new move or pause
            i = (i + 1) % 2
            display.show(groups[i])
            if i:
                frame_length = 0.4
            else:
                frame_length = 0.4
    else:
        display.show(default_group)
