# Orca - Gamecube/Wii Emulator
Orca is a Gamecube/Wii emulator targeted at FPGA-based acceleration. It can offer more accurate audio and graphics emulation where software emulation would be too slow to emulate.

Many parts of the Gamecube/Wii are not emulated with full respect to the real due to FPGA hardware and knowledge limitations but offer to use FPGA fabric to accelerate traditional methods like JIT.

Currently, the system is oriented to run on a Kria KR260, a simple, widely available, and cost-effective Xilinx Zynq Ultrascale+ board with adequate hardware specifications.

## Current Stage Of Development
The project is very far from a working version, with the majority of focus on developing the graphics pipeline from the ground up. Due to this, I will be pushing changes often as I work across many machines. It is a good resource for me or anyone else keeping up with development. Feel free to check the recent commits to see where development is.