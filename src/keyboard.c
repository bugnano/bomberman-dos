#include <dos.h>
#include <conio.h>
#include "keyboard.h"


volatile char KeyTracker[256];


static void (__interrupt __far *prev_int_09)(void);

static void __interrupt __far int_09(void);

static int ExtendedFlag;

void __interrupt __far int_09(void) {
	int scancode = inp(0x60);
	char value = 1;

	if (scancode == 0xE0) {
		// Tasto esteso
		ExtendedFlag = 1;
	} else {
		if (scancode & 0x80) {
			scancode ^= 0x80;
			value = 0;
		}

		if (ExtendedFlag) {
			ExtendedFlag = 0;
			scancode += 128;
		}

		KeyTracker[scancode] = value;
	}

	//_chain_intr(prev_int_09);
	outp(0x20, 0x20);	// Send the EOI to the PIC
}

void init_keyboard(void) {
	prev_int_09 = _dos_getvect(0x09);
	_dos_setvect(0x09, int_09);
}

void restore_keyboard(void) {
	_dos_setvect(0x09, prev_int_09);
}

