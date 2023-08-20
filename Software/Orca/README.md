# Orca Software
This is the software the runs on the Kr260's 4 core embedded A53 HIPS processor. 

In its current state, it is not setup for cross compiling and is designed to be build on the host OS itself, so in-detail build instructions will not be included.

## Building
The build folder is already present in this project, as it also includes system assets like its config, you can find guides on how to modify the config in the assets folder too.

Simply configure the cmake project from the build folder with `cmake ..` and then compile it with `make -j$(nproc)` The J option will use as many threads as needed to build the software.

After that it must be run as root with `sudo ./Orca`, as it will control the hardware directly with `/dev/mem`