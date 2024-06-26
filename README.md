# About
Graphics demos I made for playing around and learning. The code is (hopefully) clean and self-documented,
most dependencies are included for easier building.

There are no complex abstractions aka "engine", however a small library of shared code is still used.
The goal is to keep things easy to understand while taking away some of the boilerplate.

All demos are intended to run on Windows and were not tested on other systems.

Check also https://github.com/0xc0dec/demo-rs - an alternative experiment in Rust.

# Building and running
* Install Vulkan SDK. Make sure the VULKAN_SDK environment variable is set.
* `cd build`.
* `cmake -G "Visual Studio 16 2019" -A x64 ..` (or the alternative for the current MSVS at the time).
* Build using the generated IDE files.
* Run executables from `build/bin/<Debug|Release>/`.

# Controls
Some demos use first person camera. Use `WASDQE` keys to move around and hold right mouse button to rotate.

# Demos

## [Dear ImGui](/demos/imgui) [VK/GL]
Basic [Dear ImGui](https://github.com/ocornut/imgui) integration example.

![Image](/demos/imgui/screenshot.png?raw=true)

## [Transform](/demos/transform) [GL]
Object transform hierarchies and (first person) camera via reusable [`Transform`](demos/common/Transform.h) and [`Camera`](demos/common/Camera.h) classes and a helper [spectator function](demos/common/Spectator.h).

![Image](/demos/transform/screenshot.png?raw=true)

## [Skybox](/demos/skybox) [GL]
Skybox rendering on a single quad mesh using a bit of shader magic.

![Image](/demos/skybox/screenshot.png?raw=true)

## [TrueType](/demos/stb-truetype) [GL]
TrueType font rendering using [stb_truetype](https://github.com/nothings/stb) library.

![Image](/demos/stb-truetype/screenshot.png?raw=true)

## To be continued?...

# Dependencies
* stb_truetype
* stb_image
* SDL
* GLEW
* glm
* Dear ImGui
* Vulkan
* OpenGL
