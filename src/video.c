#include <i86.h>
#include <conio.h>
#include "video.h"


#define USE_BACK_BUFFER 1

static char Background[16384];


int set_video_mode(int mode) {
	int old_mode;
	union REGS regs;

	// STEP 1: Get video mode
	regs.h.ah = 0x0F;	// Get video mode
	int86(0x10, &regs, &regs);
	old_mode = regs.h.al;

	// STEP 2: Set video mode
	regs.h.ah = 0;		// Set video mode
	regs.h.al = mode;
	int86(0x10, &regs, &regs);

	return old_mode;
}

void set_palette(int palette, int bgcolor_intensity) {
	union REGS regs;

	// STEP 1: Set color palette
	regs.h.ah = 0x0B;	// Set color palette
	regs.h.bh = 1;		// 4 palette mode
	regs.h.bl = palette;
	int86(0x10, &regs, &regs);

	// STEP 2: Set palette brightness
	regs.h.ah = 0x0B;	// Set color palette
	regs.h.bh = 0;		// Brightness + background
	regs.h.bl = bgcolor_intensity;
	int86(0x10, &regs, &regs);
}

void wait_for_vblank(void) {
	while (inp(0x03DA) & 0x08) {}
	while (!(inp(0x03DA) & 0x08)) {}
}

void draw_image(int x, int y, const char* img, int w, int h) {
	int i_x;
	int i_y;
	int i_sprite;
	int i_screen;
	int start;
	int half_height = h / 2;
	int add_x;
	int rsh = (x % 4) * 2;
	int lsh = 8 - rsh;
	char tmp;
	char and1 = 0xFF << lsh;
	char and2 = 0xFF >> rsh;

#if USE_BACK_BUFFER
	char* scrptr = Background;
#else // !USE_BACK_BUFFER
	const __segment screen = 0xB800;
	char __based(screen)* scrptr = 0;
#endif // USE_BACK_BUFFER

	w /= 4;
	add_x = 80 - w;

	// Primo loop: disegno le linee pari
	start = (((y / 2) * 80) + (x / 4)) | (0x2000 * (y % 2));
	i_sprite = 0;
	i_screen = start;
	for (i_y = 0; i_y < half_height; i_y++) {
		tmp = scrptr[i_screen] & and1;
		scrptr[i_screen] = (img[i_sprite] >> rsh) | tmp;
		i_screen++;
		i_sprite++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = (img[i_sprite] >> rsh) | (img[i_sprite-1] << lsh);
			i_screen++;
			i_sprite++;
		}

		tmp = scrptr[i_screen] & and2;
		scrptr[i_screen] = (img[i_sprite-1] << lsh) | tmp;
		i_screen += add_x;
	}

	// Secondo loop: disegno le linee dispari
	i_screen = (start + ((y % 2) * 80)) ^ 0x2000;
	for (i_y = 0; i_y < half_height; i_y++) {
		tmp = scrptr[i_screen] & and1;
		scrptr[i_screen] = (img[i_sprite] >> rsh) | tmp;
		i_screen++;
		i_sprite++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = (img[i_sprite] >> rsh) | (img[i_sprite-1] << lsh);
			i_screen++;
			i_sprite++;
		}

		tmp = scrptr[i_screen] & and2;
		scrptr[i_screen] = (img[i_sprite-1] << lsh) | tmp;
		i_screen += add_x;
	}
}

void clear_area(int x, int y, int w, int h) {
	int i_x;
	int i_y;
	int i_screen;
	int start;
	int half_height = h / 2;
	int add_x;
	int rsh = (x % 4) * 2;
	int lsh = 8 - rsh;
	char and1 = 0xFF << lsh;
	char and2 = 0xFF >> rsh;

#if USE_BACK_BUFFER
	char* scrptr = Background;
#else // !USE_BACK_BUFFER
	const __segment screen = 0xB800;
	char __based(screen)* scrptr = 0;
#endif // USE_BACK_BUFFER

	w /= 4;
	add_x = 80 - w;

	// Primo loop: disegno le linee pari
	start = (((y / 2) * 80) + (x / 4)) | (0x2000 * (y % 2));
	i_screen = start;
	for (i_y = 0; i_y < half_height; i_y++) {
		scrptr[i_screen] &= and1;
		i_screen++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = 0;
			i_screen++;
		}

		scrptr[i_screen] &= and2;
		i_screen += add_x;
	}

	// Secondo loop: disegno le linee dispari
	i_screen = (start + ((y % 2) * 80)) ^ 0x2000;
	for (i_y = 0; i_y < half_height; i_y++) {
		scrptr[i_screen] &= and1;
		i_screen++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = 0;
			i_screen++;
		}

		scrptr[i_screen] &= and2;
		i_screen += add_x;
	}
}

void read_image(int x, int y, char* img, int w, int h) {
	int i_x;
	int i_y;
	int i_sprite;
	int i_screen;
	int start;
	int half_height = h / 2;
	int add_x;
	int lsh = (x % 4) * 2;
	int rsh = 8 - lsh;

#if USE_BACK_BUFFER
	char* scrptr = Background;
#else // !USE_BACK_BUFFER
	const __segment screen = 0xB800;
	char __based(screen)* scrptr = 0;
#endif // USE_BACK_BUFFER

	w /= 4;
	add_x = 80 - w;

	// Primo loop: leggo le linee pari
	start = (((y / 2) * 80) + (x / 4)) | (0x2000 * (y % 2));
	i_sprite = 0;
	i_screen = start;
	i_screen++;
	for (i_y = 0; i_y < half_height; i_y++) {
		for (i_x = 0; i_x < w; i_x++) {
			img[i_sprite] = (scrptr[i_screen] >> rsh) | (scrptr[i_screen-1] << lsh);
			i_screen++;
			i_sprite++;
		}
		i_screen += add_x;
	}

	// Secondo loop: leggo le linee dispari
	i_screen = (start + ((y % 2) * 80)) ^ 0x2000;
	i_screen++;
	for (i_y = 0; i_y < half_height; i_y++) {
		for (i_x = 0; i_x < w; i_x++) {
			img[i_sprite] = (scrptr[i_screen] >> rsh) | (scrptr[i_screen-1] << lsh);
			i_screen++;
			i_sprite++;
		}
		i_screen += add_x;
	}
}

void update_screen(int x, int y, int w, int h) {
#if USE_BACK_BUFFER
	int i_x;
	int i_y;
	int i_screen;
	int start;
	int half_height = (h + 1) / 2;
	int add_x;

	const __segment screen = 0xB800;
	char __based(screen)* scrptr = 0;

	w = (w + 3) / 4;
	add_x = 80 - w;

	// Primo loop: disegno le linee pari
	start = (((y / 2) * 80) + (x / 4)) | (0x2000 * (y % 2));
	i_screen = start;
	for (i_y = 0; i_y < half_height; i_y++) {
		scrptr[i_screen] = Background[i_screen];
		i_screen++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = Background[i_screen];
			i_screen++;
		}

		scrptr[i_screen] = Background[i_screen];
		i_screen += add_x;
	}

	// Secondo loop: disegno le linee dispari
	i_screen = (start + ((y % 2) * 80)) ^ 0x2000;
	for (i_y = 0; i_y < half_height; i_y++) {
		scrptr[i_screen] = Background[i_screen];
		i_screen++;

		for (i_x = 1; i_x < w; i_x++) {
			scrptr[i_screen] = Background[i_screen];
			i_screen++;
		}

		scrptr[i_screen] = Background[i_screen];
		i_screen += add_x;
	}
#else // !USE_BACK_BUFFER
#endif // USE_BACK_BUFFER
}

