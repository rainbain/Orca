# Orca - Gamecube/Wii Emulator
Orca is a Gamecube/Wii emulator targeted at FPGA-based acceleration. It can offer more accurate audio and graphics emulation where software emulation would be too slow to emulate.

Many parts of the Gamecube/Wii are not emulated with full respect to the real due to FPGA hardware and knowledge limitations but offer to use FPGA fabric to accelerate traditional methods like JIT.

Currently, the system is oriented to run on a Kria KR260, a simple, widely available, and cost-effective Xilinx Zynq Ultrascale+ board with adequate hardware specifications.

## Current Stage Of Development
The project is very far from a working version, with the majority of focus on developing the graphics pipeline from the ground up. Due to this, I will be pushing changes often as I work across many machines. It is a good resource for me or anyone else keeping up with development. Feel free to check the recent commits to see where development is.

## Building
The current state of the code is not in a build ready version.
With that, you can find build instructions around parts of the project, mainly in the `Software` folder, though they may be tediouse and or incomplete.

### The Simulator
The simulator is a verilator project that simulates Orca and its components to provide a idintical API to the real hardware. It is very slow as it simulates Orca at the RTL level.

### RTL
This is the verilog code shared across the real hardware and simulator.

### Docs
Data I have collected about the Gamecube/Wii or just notes about Orca, like possible missing functions in already existing hardware.

### Software
This contains the actual source code to the Software/API designed to run on the real SoC as well as Orca's Boot and any other components that Orca may run on the real platform.