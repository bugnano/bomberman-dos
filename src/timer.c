#include <dos.h>
#include <conio.h>
#include "timer.h"


extern int* Music;
extern long IMusic;
extern long LenMusic;


volatile int ClockTicks;

static void (__interrupt __far *prev_int_08)(void);

static void __interrupt __far int_08(void);

static int PrevPort61;

void __interrupt __far int_08(void) {
	ClockTicks++;

	outp(0x43, 0xB6);
	outp(0x42, Music[IMusic] & 0xFF);
	outp(0x42, Music[IMusic] >> 8);
	IMusic++;
	if (IMusic >= LenMusic) {
		IMusic = 0;
	}


	if ((ClockTicks % 4) == 0) {
		_chain_intr(prev_int_08);
	} else {
		outp(0x20, 0x20);	// Send the EOI to the PIC
	}
}

void init_timer(void) {
	// (65536 / 4) = 16384
	int freq = 16384;

	prev_int_08 = _dos_getvect(0x08);
	_dos_setvect(0x08, int_08);

	outp(0x43, 0x36);
	outp(0x40, freq & 0x0F);
	outp(0x40, freq >> 8);
}

void restore_timer(void) {
	// Ripristino il valore originale
	outp(0x43, 0x36);
	outp(0x40, 0);
	outp(0x40, 0);

	_dos_setvect(0x08, prev_int_08);
}

void init_beeper(void) {
	PrevPort61 = inp(0x61);

	outp(0x61, PrevPort61 | 0x03);
}

void restore_beeper(void) {
	outp(0x61, PrevPort61 & 0xFC);
}

