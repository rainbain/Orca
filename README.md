# Orca - Gamecube/Wii Emulator

Orca is a Gamecube/Wii Emulator allowing for the running of related games on a FPGA. Development is
mostly focused on the emulation of the graphics and audio system. Full system emulation is far away if possible.


## Current Stage Of Development

The current stage of development takes place on a classic De10-Nano FPGA. This is only about enough
to fit the Gamecubes XF, a very small portion of the whole graphics pipeline, and 100 MHz under the
goal clock frequency. Current goals are to move to a ku5p development board once one is found as
it is hoped to be able to contain all of Flipper.

The current state of the code is not functional. I am working hard to change that!


## Why?

The Gamecubes graphics, Flipper, as well as its surrounding hardware, requires high end FPGAs to
be implemented, which begs the question of why hardware emulation for these systems should exist.

The main goals of Orca are to provide as close to the hardware as possible in a goal to preserve
the original function of the Gamecube as well as its greatly unique deferred lighting engine.
It also provides an interesting challenge as hardware such as the Gamecube is normal out of
the question for traditional Gamecube emulation.


## Can it be done?

The Gamecube's large cache, clock speeds, and memory bandwidth are all major issues in
the future development of Orca. Running the hardware at the intended speed is a goal 
of Orca. With that, the PowerPC 750CXe core is currently outside the goal of emulation.
Current work involves offloading parts of Dolphin, a popular Gamecube/Wii emulator, to
Orca to test the accuracy of its emulation and work out from there.


## Final

I am not an experienced RTL engineer nor have long-term experience in Verilog.
If you see anything that is odd or wish to contribute your knowledge in some way I am open
to all suggestions regarding Orca. A large part of this project is to provide
a long-term learning experience in the field I plan to work in.