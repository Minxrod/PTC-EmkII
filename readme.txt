PTC Interpreter, attempt two!

== Introduction ==

This project is an attempt at recreating the mechanics of Petit Computer, to be able to run PTC programs on PC. The end goal is to support all commands and functions that are usable from programs. Accuracy will likely not be 100%, though most programs should run as expected. Note that bugs will probably not be recreated, so stuff like -0 or the COLSET bug/crash will not work the same.

Please note that this is heavily a WIP and nowhere near finished, so don't be surprised if something breaks.

== Support ==

The current project has some amount of support for samples ENG1-ENG12, and ENG1G-ENG5G.

= Samples =

- Sample 1: Works.
- Sample 2: Works, but does not handle invalid input.
- Sample 3: Works.
- Sample 4: Works.
- Sample 5: Works.
- Sample 6: Partially works, but suffers slowdown the more notes are played.
- Sample 7: Works.
- Sample 8: Works.
- Sample 9: Works.
- Sample 10: Works.
- Sample 11: Works.
- Sample 12: Not quite working - audio does not match, saving/loading does not seem to work.
- Game 1: Works.
- Game 2: Works, but input is slightly off (occasional double-inputs).
- Game 3: Works.
- Game 4: Works.
- Game 5: Works.
- CHRED: Untested, likely broken.
- SCRED: Untested, likely broken.
- GRPED: Untested, likely broken.
- DRWED: Untested, likely broken.

= Features =

- Console: Most commands on the console should be working, but there are issues with INPUT and LINPUT. 
- Background: Most operations should work, might be missing a few alternate forms of commands.
- Sprites: Pretty good support; probably missing some more unusual features.
- Graphics: Most operations should work. GPAINT is unimplemented, some variants of other commands may be missing.
- Panel: In progress, does not yet support ICON commands.
- Files: Default graphics work; Some loading and saving has been implemented, but is very untested.
- Sound: Alright, but has some differences from the original. Does not implement BGMSETV, BGMGETV yet.
- Input: Buttons and keyboard work, experimental controller support. Controls can be changed from config/controls.txt, though it's not yet convenient.

== Building ==

Requirements: 
* SFML 2.5.1
* g++ supporting c++17 (c++1z may work as well, with some effort)
* make or something compatible
* Python 3 (for extraction scripts, technically optional if you manually perform the extraction)

= Linux =

Clone this repo, then create a build directory named "build/" in the root of the repo. Ensure you have SFML installed, and run make. It should work (currently untested on other machines).
If you don't have SFML installed, go here first: https://www.sfml-dev.org/tutorials/2.5/start-linux.php

Commands will probably look something like this:
$ git clone https://github.com/Minxrod/PTC-EmkII.git
$ cd PTC-EmkII
$ mkdir build/
$ make

= Windows =

Instructions should be roughly the same as linux, but may require some modifications to the makefile. It is expected you have git installed and accessible from the command line.
It is suggested you use the version with MinGW 7.3.0 32-bit linked from https://www.sfml-dev.org/download/sfml/2.5.1/, as it is known to work.

If you install SFML to a location other than C:/SFML-2.5.1/, modify the SFML_PATH variable in the makefile to reflect this.

Then, the instructions should be similar to
$ git clone https://github.com/Minxrod/PTC-EmkII.git
$ cd PTC-EmkII
$ mkdir build/
$ mingw32-make

Additionally, you will need to move the relevant .dll files to the project root. You can find these in your SFML installation, in the bin folder.
If you used the default directory, you can use the following commands:

$ cp C:\SFML-2.5.1\bin\* .
$ cp lib\SSEQPlayer.dll .

= Documentation =

To build the documentation, install doxygen and use
$ doxygen doxyconfig

== Usage == 

= Linux setup =

Place a PTC .nds file in the resources/ directory, then run

$ cd resources/
$ ./nds_extract <ptc_file>.nds

This should automatically extract all required graphics and sound resources to the correct locations. Note that if you put more than one .nds in the resources directory, it will not work.

If the precompiled versions of ndstool or sdatxtract do not work, see the Tools link below and compile/download the binaries yourself, and replace the copies in resources/tools/.

= Windows setup =

Place a PTC .nds file in the resources/ directory, then run

$ cd resources/
$ nds_extract.bat <ptc_file>.nds

If the precompiled versions of ndstool or sdatxtract do not work, see the Tools link below and compile/download the binaries yourself, and replace the copies in resources/tools/.

Put any PTC files you want to run or load in the programs/ folder. This includes resources, such as CHR or GRP files.
You will be prompted to enter a filename when launching PTC-EmkII.

= Execution options =

-s = Disable sound system entirely. This also disables BGMCHK and the like, so is not recommended if your program needs this.
-d = Enable a simple debugger. Currently, lets you track simple variables, set breakpoints, and view CHR resource textures.
-a = Enable automatic reload on program exit. When a PTC program ends, this will reload the startup loader program.

= Debugger =

The debugger is a second window that can be opened on execution with the -d option, or opened mid-executation with F12.

Currently, the options are as follows:

tr expression - Add an expression to the list
rm expression - Remove an expression from the list
exit - Close the debugger window
br line - Sets a breakpoint at line
clear line - Removes a breakpoint at line
c - Continue if program execution is paused from a breakpoint
tex index - Displays the resource corresponding to the given index.

Note that expressions are updated per-frame, not as values change. Also, note that functions, operations and variable access will work in these expressions, but that attempting to create a new variable is not currently supported and will likely crash due to thread-safety issues.

= Controls: =

- WASD = dpad
- Arrow Keys = ABXY
- Q = L
- E = R
- Enter = Start
- RShift = Select

Special keys:
- F1: Zoom x1
- F2: Zoom x2
- F5: Restart current program (Note: Will wait until end of current instruction to take effect, will not work if program has already terminated)
- F10: Disable/Enable keyboard buttons
- F11: Disable/Enable keyboard 
- F12: Open debugger window

KEYBOARD and INKEY$() are approximated by typing on the keyboard. The touch keyboard is partially functional, but the different keyboards are not yet implemented.

== Tools used ==

- https://github.com/Oreo639/sdatxtract
- https://github.com/devkitPro/ndstool

Licenses for these are located in resources/tools/ along with the binaries.

== Libraries used ==

- https://create.stephan-brumme.com/hash-library/ (under zlib license)
- www.sfml-dev.org (under zlib license)

== Resources/Links ==

- https://en.cppreference.com
- www.sfml-dev.org
- https://stackoverflow.com
- https://petitcomputer.fandom.com/wiki/Petit_Computer_Wiki
- https://micutil.com/ptcutilities/top_e.html
- Whatever is linked in the source
- And probably more that has been forgotten.
