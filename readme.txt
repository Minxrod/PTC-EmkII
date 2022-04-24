PTC Interpreter, attempt two!

== Description ==

This project is an attempt at recreating the mechanics of Petit Computer, to be able to run PTC programs on PC. The end goal is to support all commands and functions that are usable from programs - DIRECT mode is not currently planned to be supported. Accuracy will likely not be 100%, though most programs should run as expected. Note that bugs will probably not be recreated, so stuff like -0 or the COLREAD bug/crash will not work the same.

Please note that this is heavily a WIP and nowhere near finished, so don't expect anything to work.

= Support =
The current project has some amount of support for samples ENG1.PTC-ENG7.PTC:
- Sample 1: Works.
- Sample 2: Works, but does not handle invalid input.
- Sample 3: Works.
- Sample 4: Partially works, RND is currently unseeded and always returns the same values.
- Sample 5: Partially works, values/graphics are off by a small amount?
- Sample 6: Partially works, only the top screen is currently rendered. (No icon = no eraser)
- Sample 7: Works.
- Sample 8: Partially works, SWAP, SORT, RSORT unimplemented
- Sample 9: Mostly does not work.
- Sample 10: Mostly does not work.
- Sample 11: Does not work.
- Sample 12: Does not work.
- Game 1: Works.
- Game 2: Works, but input feels slow.
- Game 3: Works, sometimes? Runs slowly compared to original, needs more testing.
- Game 4: Works, for some reason the menu doesn't quite work?
- Game 5: Works.

= Features =
- Console: Almost everything on the console should be working, issues only with INPUT and LINPUT.
- Background: Basic operations should work, BGREAD and BGCOPY are not finished.
- Sprites: Partial support; several visual elements are working.
- Graphics: Some basic operations should work, very unfinished.
- Panel: In progress, does not yet support ICON commands.
- Files: Only default graphics are currently loaded.
- Sound: In progress, but has some differences from the original.
- Input: Buttons and keyboard work, experimental controller support. Controls can be changed from config/controls.txt, though it's not yet convenient.

== Building ==

= Linux =

Clone this repo, then create a build directory named "build/" in the root of the repo. Ensure you have SFML installed, and run make. It should work (currently untested on other machines).
If you don't have SFML installed, go here first: https://www.sfml-dev.org/tutorials/2.5/start-linux.php

Commands will probably look something like this:
$ git clone https://github.com/Minxrod/PTC-EmkII.git
$ cd PTC-EmkII
$ mkdir build/
$ make

== Instructions == 

= Setup =

Place a PTC .nds file in the resources/ directory, then run

$ cd resources/
$ ./nds_extract

This should automatically extract all required graphics and sound resources to the correct locations. Note that if you put more than one .nds in the resources directory, it will not work.
This has only been tested on Debian 11. It is currently UNTESTED on other machines; I do not know if the copy of ndstool or sdatxtract provided will work. If it does not, see the Tools link below and compile/download the binaries yourself, and replace the copies in resources/tools/.

Put any PTC files you want to run or load in the programs/ folder. This includes resources, such as CHR or GRP files.
You will be prompted to enter a filename when launching PTC-EmkII.

= Controls: =

- WASD = dpad
- Arrow Keys = ABXY
- Q = L
- Numpad1 = R
- Enter = Start
- RShift = Select

Special keys:
- Home: Disable/Enable keyboard
- PageUp: Zoom x1
- PageDown: Zoom x2
- End: Disable/Enable keyboard buttons

KEYBOARD and INKEY$() are approximated by typing on the keyboard. Note that currently, not all keys are mapped, so some symbols are currently unable to be typed (ex. $ or %)

== Tools used ==

- https://github.com/Oreo639/sdatxtract
- https://github.com/devkitPro/ndstool

== Resources/Links ==

- https://en.cppreference.com
- www.sfml-dev.org
- https://stackoverflow.com
- https://petitcomputer.fandom.com/wiki/Petit_Computer_Wiki
- https://micutil.com/ptcutilities/top_e.html
- Whatever is linked in the source
- And probably more that has been forgotten.
