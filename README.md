# About
Graphics demos made for learning and playing around with. The code is (hopefully) clean and self-documented,
all dependencies are included for easier building.

# Building and running
* `mkdir build && cd build`.
* `cmake -G "Visual Studio 16 2019" -A x64 ..` (or other generator depending on the IDE).
* Build using the generated IDE files.
* Run executables inside `build/<Debug|Release>/`.

# Demos
## Transform
Features reusable `Transform` and `Camera` classes for working with object hierarchies and (first person) camera.

![Image](/screenshots/transform.png?raw=true)

## Skybox
Rendering skybox on a single quad mesh with a bit of shader magic.

![Image](/screenshots/skybox.png?raw=true)

## TrueType
Rendering TrueType font using [stb_truetype](https://github.com/nothings/stb) library.

![Image](/screenshots/stb-truetype.png?raw=true)

## To be continued...

# Dependencies
* stb_truetype
* SDL
* GLEW
* glm
