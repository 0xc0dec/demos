## About
A number of graphics demos made for learning and playing around with. The code is (hopefully) clean and self-documented,
all dependencies are included.

## Building and running
* `mkdir build && cd build`.
* `cmake -G "Visual Studio 16 2019" -A x64 ..` (or other generator depending on the IDE).
* Build using the generated IDE files.
* Run executables inside `build/<Debug|Release>/`.

## Demos
* `StbTrueType.cpp`: rendering TrueType font in OpenGL using [stb_truetype](https://github.com/nothings/stb) library.

![1](/screenshots/StbTrueType.png?raw=true)

* `Transform.cpp`: using `Transform` and `Camera` classes for working with transform hierarchies and first person camera.

![1](/screenshots/Transform.png?raw=true)

* To be continued...

## Dependencies
* stb_truetype
* SDL
* GLEW
* glm
