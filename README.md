# About
Graphics demos made for learning and playing around with. The code is (hopefully) clean and self-documented,
all dependencies are included for easier building.

There are no complex abstractions aka "engine", but a small library of shared code is still used.
The goal is to keep things easy to understand while taking away as much boilerplate stuff as possible.

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
* stb_image
* SDL
* GLEW
* glm
