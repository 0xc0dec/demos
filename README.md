## About
A (hopefully) self-explanatory example demonstrating how to render TrueType fonts in OpenGL 3+ with the help of
[stb_truetype](https://github.com/nothings/stb) library.

The essential part is contained in `Example1.cpp` and `Example2.cpp` files. Directories `vendor` and `common` hold
platform-dependent code needed for the example to run and are not required for understanging how the `stb_truetype` works.
The repository contains all dependencies needed to compile demos on Windows.

![1](/Example1.png?raw=true)
![1](/Example2.png?raw=true)

## Dependencies
* stb_truetype
* SDL
* GLEW
