# An unfinished Bomberman clone for DOS

This is a hobby project that I started some years ago and never finished.

It's a clone of the Bomberman game meant to run on:

* DOS (Tested on MS-DOS 3.3)
* i8086 CPU (or i8088)
* 128k RAM
* CGA graphics
* PC Speaker sound
* Keyboard input

## Extracting the graphics

In the `tools/` directory, run the `extract_tiles.py` script first, in order to obtain 1 PNG image for each frame.

After that, run the `make_cga.py` script, in order to convert the PNG images to sprite-sheets compatible with CGA.

The tools require Python 3 and Pillow, and have been tested on Linux.

## Changing the music (optional)

The music is generated with [MONOTONE](https://github.com/MobyGamer/MONOTONE).

* From MONOTONE, load the song, and press CTRL-W to dump the song to file.
* Copy the dumped file by overwriting the `out/game.snd` file.

## Compiling

The compiler used to compile this game is the Open Watcom C/C++ compiler for DOS,
and you can easily obtain it from [FreeDOS](https://freedos.org/) (Look for the `WATCOMC.ZIP` file).

The Open Watcom C/C++ compiler works on DOSBox, and it's the easiest way to compile the game.

From DOSBox run the commands:

```
C:\DEVEL\WATCOMC\OWSETENV.BAT
wmake /a
```

## Running the game

Copy the `out` directory to the target DOS machine, and run with `GAME.EXE`.

Of course, it runs on DOSBox as well.

* To move the player press the ARROW KEYS
* To place a bomb press SPACE
* To quit the game press ESC or Q

## Useful links

How to program the CGA graphics:
https://www.seasip.info/VintagePC/cga.html
https://www.chibialiens.com/8086/platform.php#LessonP1

PC Game Programmer's Encyclopedia:
http://qzx.com/pc-gpe/

How to program the Keyboard:
http://qzx.com/pc-gpe/keyboard.txt

How to program the PC Speaker:
http://qzx.com/pc-gpe/speaker.txt

How to program the Timer:
http://qzx.com/pc-gpe/pit.txt

Useful information about hardware ports and interrupts:
https://www.stanislavs.org/helppc/

Open Watcom documentation:
https://github.com/open-watcom/open-watcom-1.9/wiki/Open-Watcom-1.9-Documentation

Open Watcom C/C++ for DOS:
https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/repositories/1.3/pkg-html/watcomc.html

## License

GPL-3.0-or-later
