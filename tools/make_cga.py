#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os

from pprint import pprint

from PIL import Image


IN_FILES = {
	'../out/bomb.sht': [
		'tiles/tile_42.png',
		'tiles/tile_43.png',
		'tiles/tile_44.png',
	],
	'../out/brick.sht': [
		'tiles/tile_46.png',
		'tiles/tile_47.png',
		'tiles/tile_48.png',
		'tiles/tile_49.png',
		'tiles/tile_50.png',
		'tiles/tile_51.png',
		'tiles/tile_52.png',
	],
	'../out/edd.sht': [
		'tiles/tile_114.png',
		'tiles/tile_119.png',
		'tiles/tile_184.png',
		'tiles/tile_189.png',
	],
	'../out/ell.sht': [
		'tiles/tile_84.png',
		'tiles/tile_89.png',
		'tiles/tile_154.png',
		'tiles/tile_159.png',
	],
	'../out/emd.sht': [
		'tiles/tile_100.png',
		'tiles/tile_105.png',
		'tiles/tile_170.png',
		'tiles/tile_175.png',
	],
	'../out/eml.sht': [
		'tiles/tile_85.png',
		'tiles/tile_90.png',
		'tiles/tile_155.png',
		'tiles/tile_160.png',
	],
	'../out/emm.sht': [
		'tiles/tile_86.png',
		'tiles/tile_91.png',
		'tiles/tile_156.png',
		'tiles/tile_161.png',
	],
	'../out/emr.sht': [
		'tiles/tile_87.png',
		'tiles/tile_92.png',
		'tiles/tile_157.png',
		'tiles/tile_162.png',
	],
	'../out/emu.sht': [
		'tiles/tile_72.png',
		'tiles/tile_77.png',
		'tiles/tile_142.png',
		'tiles/tile_147.png',
	],
	'../out/err.sht': [
		'tiles/tile_88.png',
		'tiles/tile_93.png',
		'tiles/tile_158.png',
		'tiles/tile_163.png',
	],
	'../out/euu.sht': [
		'tiles/tile_58.png',
		'tiles/tile_63.png',
		'tiles/tile_128.png',
		'tiles/tile_133.png',
	],
	'../out/wall.sht': [
		'tiles/tile_45.png',
	],
	'../out/wk_dn.sht': [
		'tiles/tile_3.png',
		'tiles/tile_4.png',
		'tiles/tile_5.png',
	],
	'../out/wk_lt.sht': [
		'tiles/tile_0.png',
		'tiles/tile_1.png',
		'tiles/tile_2.png',
	],
	'../out/wk_rt.sht': [
		'tiles/tile_14.png',
		'tiles/tile_15.png',
		'tiles/tile_16.png',
	],
	'../out/wk_up.sht': [
		'tiles/tile_17.png',
		'tiles/tile_18.png',
		'tiles/tile_19.png',
	],
}

TILE_WIDTH = 16
TILE_HEIGHT = 16


def ByteFromImage(src_image, x, y):
	b = 0

	c = src_image.getpixel((x, y))
	if c == (0, 255, 255, 255):
		b |= 0x01 << 6
	elif c == (255, 0, 255, 255):
		b |= 0x02 << 6
	elif c == (255, 255, 255, 255):
		b |= 0x03 << 6

	c = src_image.getpixel((x+1, y))
	if c == (0, 255, 255, 255):
		b |= 0x01 << 4
	elif c == (255, 0, 255, 255):
		b |= 0x02 << 4
	elif c == (255, 255, 255, 255):
		b |= 0x03 << 4

	c = src_image.getpixel((x+2, y))
	if c == (0, 255, 255, 255):
		b |= 0x01 << 2
	elif c == (255, 0, 255, 255):
		b |= 0x02 << 2
	elif c == (255, 255, 255, 255):
		b |= 0x03 << 2

	c = src_image.getpixel((x+3, y))
	if c == (0, 255, 255, 255):
		b |= 0x01 << 0
	elif c == (255, 0, 255, 255):
		b |= 0x02 << 0
	elif c == (255, 255, 255, 255):
		b |= 0x03 << 0

	return b


def main():
	for (out_file, in_files) in IN_FILES.items():
		with open(out_file, 'wb') as fp:
			for in_file in in_files:
				src_image = Image.open(in_file)
				w, h = src_image.size

				for y in range(0, h, 2):
					for x in range(0, w, 4):
						b = ByteFromImage(src_image, x, y)
						fp.write(bytes.fromhex('{:02X}'.format(b)))

				for y in range(1, h, 2):
					for x in range(0, w, 4):
						b = ByteFromImage(src_image, x, y)
						fp.write(bytes.fromhex('{:02X}'.format(b)))


if __name__ == '__main__':
	main()

