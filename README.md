# CppStation

Playstation emulator in C++ language.

The aim of the project is an attempt to learn the process of emulation the Playstation by rewriting an existing code of the [Rustation](https://github.com/simias/rustation) emulator, following the [psx-guide](https://github.com/simias/psx-guide).

I don't take any credit for the original design of the project / the original code, full credit goes to [simias](https://github.com/simias).

# Current status

Project is at a very early stage of the development, not all CPU opcodes are implemented yet.

# Build

```
mkdir build
cd build
cmake ..
make -j8
```

For the moment only Linux (x86-64) is supported. Project uses a bit of platform dependent code to ease the debugging process on Linux.                    
Support for other OS like Windows shouldn't be too difficult to do, with a removal or rewriting that debugging code in the future.   


You will also need the Playstation BIOS.
The emulator is mainly tested with BIOS version SCPH1001 whose SHA-1 is 10155d8d6e6e832d6ea66db9bc098321fb5e8ebf.

# Disclaimer

"PlayStation" and "PSX" are registered trademarks of Sony Interactive Entertainment Europe Limited. This project is not affiliated in any way with Sony Interactive Entertainment.
