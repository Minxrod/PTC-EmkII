PTC Interpreter, attempt two!

== Description ==

This project is an attempt at recreating the mechanics of Petit Computer, to be able to run PTC program on PC. The end goal is to support all commands and functions that are usable from programs - DIRECT mode is not currently planned to be supported. Accuracy will likely not be 100%, though most programs should run as expected. Note that bugs will probably not be recreated, so stuff like -0 or the COLREAD bug/crash will not work the same.

Please note that this is heavily a WIP and nowhere near finished, so don't expect anything to work.

= Support =
The current project has some amount of support for samples ENG1.PTC-ENG7.PTC:
- Sample 1: Works.
- Sample 2: Partially working, some keys are not mapped.
- Sample 3: Partially working, same as Sample 2.
- Sample 4: Partially working, RND is currently unseeded and always returns the same values.
- Sample 5: Works, same issues as Sample 2. No unmapped inputs are required, though.
- Sample 6: Partially working, holding down the mouse does not seem to work and only the top screen is currently rendered.
- Sample 7: Works, same issues as Sample 5.
- Sample 8-12: Not working, require various unimplemented features.

= Features =
- Console: Almost everything on the console should be working, issues only with INPUT and LINPUT.
- Background: Basic operations should work, BGREAD and BGCOPY are not finished.
- Sprites: Does not exist yet.
- Graphics: Some basic operations should work, very unfinished.
- Panel: Does not exist yet, nowhere near completion (likely will be built off of background and sprite support)
- Files: Only default graphics are currently loaded.
- Sound: Beeps work aside from panpot, BGM is not implemented yet.
- Input: Partial keyboard, WASD/Arrow controls.

== Building ==

= Linux =

Clone this repo, then create a build directory named "build/" in the root of the repo. Ensure you have SFML installed, and run make. It should work (currently untested on other machines).
If you don't have SFML installed, go here first: https://www.sfml-dev.org/tutorials/2.5/start-linux.php

Commands will probably look something like this:
$ git clone https://github.com/Minxrod/PTC-EmkII.git
$ cd PTC-EmkII
$ mkdir build/
$ make

= Windows = 

To be finished later, currently trying to resolve issues with compatibility (likely will require several small changes to source). 

WIP instructions:
- Install MSYS, instructions at www.msys2.org.
- Download/clone this repo, then create a build folder named "build/" in the root of the repo.
- Download SFML 2.5.1 here: https://www.sfml-dev.org/download/sfml/2.5.1/
- Add the following option to the end of OFLAGS: -I<your SFML include path> Example: -IC:\SFML-2.5.1\include
- Run make, you will get several errors. (currently being worked on)

== Instructions == 

= Setup =

You will need the default graphics (CHR+COL) files. These are resources such as SPU0, BGD1, COL0, etc. and can be obtained from a copy of PTC easily. Place these files, named as "resource.PTC" (ex. SPU0.PTC) in the folder resources/graphics/.

For sounds to work, you will need sound files named beepXXX.wav, where X is a 3 digit waveform number. These files should be placed in resources/sounds/sfx/. These are somewhat more involved to obtain; a guide for recording sounds will be finished later.

Put any PTC files you want to run/load in the programs/ folder.
You will be prompted to enter a filename when launching PTC-EmkII.

= Controls: =

- WASD = dpad
- Arrow Keys = ABXY
- Q = L
- Numpad1 = R
- Enter = Start
- RShift = Select

KEYBOARD and INKEY$() are approximated by typing on the keyboard. Note that currently, not all keys are mapped, so some symbols are currently unable to be typed (ex. $ or %)

== Resources/Links ==

- https://en.cppreference.com
- www.sfml-dev.org
- https://stackoverflow.com
- https://petitcomputer.fandom.com/wiki/Petit_Computer_Wiki
- https://micutil.com/ptcutilities/top_e.html
- Whatever is linked in the source
- And probably more that has been forgotten.
