#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os

from PIL import Image


FILE_IN = '7884_cga.png'
FMT_OUT = 'tiles/tile_%u.png'

TILE_WIDTH = 16
TILE_HEIGHT = 16

def main():
	os.makedirs('tiles')

	src_image = Image.open(FILE_IN)
	w, h = src_image.size

	i = 0
	for y in range(int(h / TILE_HEIGHT)):
		for x in range(int(w / TILE_WIDTH)):
			frame = src_image.crop((x * TILE_WIDTH, y * TILE_HEIGHT, (x * TILE_WIDTH) + TILE_WIDTH, (y * TILE_HEIGHT) + TILE_HEIGHT))
			out_image = Image.new('RGBA', (TILE_WIDTH, TILE_HEIGHT), (0, 0, 0, 0))
			out_image.paste(frame, (0, 0))
			out_image.save(FMT_OUT % (i))
			i += 1


if __name__ == '__main__':
	main()

