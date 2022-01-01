# CppStation

Playstation emulator in C++ language.

The aim of the project is an attempt to learn the process of emulation the Playstation by rewriting an existing code of the [Rustation](https://github.com/simias/rustation) emulator, following the [psx-guide](https://github.com/simias/psx-guide).

I don't take any credit for the original design of the project / the original code, full credit goes to [simias](https://github.com/simias).

# Current status

Project is at a very early stage of the development.
Currently all the CPU opcodes are implemented, but the emulator doesn't handle any of the peripherals (DMA, SPU, GPU etc.).

# Build

For Linux, glfw3 and OpenGL dev packages are required.

```
mkdir build
cd build
cmake ..
make -j8
```

Builds on Windows can be done in VSCode IDE with vcpkg package manager.

You will also need the Playstation BIOS.
The emulator is mainly tested with BIOS version SCPH1001 whose SHA-1 is 10155d8d6e6e832d6ea66db9bc098321fb5e8ebf.

# Disclaimer

"PlayStation" and "PSX" are registered trademarks of Sony Interactive Entertainment Europe Limited. This project is not affiliated in any way with Sony Interactive Entertainment.
