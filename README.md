# About
Graphics demos made for learning and playing around with. The code is (hopefully) clean and self-documented,
all dependencies are included for easier building.

Demos do not use any complex dedicated engine code (at least yet), however some shared library code is still present.
Its goal is to keep each demo easy to understand and close to the graphics API while not bloating it with lots of boilerplate stuff.

# Building and running
* `mkdir build && cd build`.
* `cmake -G "Visual Studio 16 2019" -A x64 ..` (or other generator depending on the IDE).
* Build using the generated IDE files.
* Run executables inside `build/<Debug|Release>/`.

# Demos
## Transform
Object transform hierarchies and (first person) camera via reusable `Transform` and `Camera` classes.

![Image](/screenshots/transform.png?raw=true)

## Skybox
Skybox rendering on a single quad mesh using a bit of shader magic.

![Image](/screenshots/skybox.png?raw=true)

## TrueType
TrueType font rendering using [stb_truetype](https://github.com/nothings/stb) library.

![Image](/screenshots/stb-truetype.png?raw=true)

## To be continued...

# Dependencies
* stb_truetype
* SDL
* GLEW
* glm
