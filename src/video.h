#ifndef _VIDEO_H_
#define _VIDEO_H_

#define CGA_MODE_04		0x04	// Mode 0x04 (CGA 320x200 4 colors)
#define CGA_MODE_05		0x05	// Mode 0x05 (CGA 320x200 4 colors)
#define EGA_MODE_0D		0x0D	// Mode 0x0D (EGA 320x200 16 colors)
#define VGA_MODE_13		0x13	// Mode 0x13 (VGA 320x200 256 colors)

#define PALETTE_WARM	0
#define PALETTE_COOL	1

#define LOW_INTENSITY	0x11
#define HIGH_INTENSITY	0x10

#define BLACK			0
#define DARK_BLUE		1
#define DARK_GREEN		2
#define DARK_CYAN		3
#define DARK_RED		4
#define DARK_MAGENTA	5
#define BROWN			6
#define LIGHT_GRAY		7
#define DARK_GRAY		8
#define LIGHT_BLUE		9
#define LIGHT_GREEN		10
#define LIGHT_CYAN		11
#define LIGHT_RED		12
#define LIGHT_MAGENTA	13
#define YELLOW			14
#define WHITE			15

int set_video_mode(int mode);

void set_palette(int palette, int bgcolor_intensity);

void wait_for_vblank(void);

void draw_image(int x, int y, const char* img, int w, int h);
void clear_area(int x, int y, int w, int h);
void read_image(int x, int y, char* img, int w, int h);
void update_screen(int x, int y, int w, int h);

#endif // _VIDEO_H_

