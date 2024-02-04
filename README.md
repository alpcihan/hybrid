# Hybrid

**Experimental real-time hybrid renderer for triangular geometry and SDF rendering made with C++ and Vulkan**

The main motivation of the project is to render triangular geometry and Signed Distance Functions (SDFs) with a unified lighting system, supported by hardware-accelerated ray tracing.

<img width="960" alt="hybrid_png" src="https://github.com/alpcihan/hybrid/assets/37274614/b6f1394d-8eef-4bfe-9c03-e59fa7a6423f">

*Captured from the [fractal_flythrough](./examples/fractal_flythrough) example.*

*[Apollonian](https://www.shadertoy.com/view/4ds3zn) fractal by [iq](https://www.shadertoy.com/user/iq), [FA-59A Mako](https://www.turbosquid.com/3d-models/fa-59a-mako-1078468) spaceship model by [hefturush](https://www.turbosquid.com/Search/Artists/hefturush) and [Village Cloudy Sky Dome](https://hdri-haven.com/hdri/village-cloudy-sky-dome) HDRI from [hdri-haven](https://hdri-haven.com).*

## Features

- [x] Hybrid rendering using a mixture of rasterization, ray tracing, ray marching and compute shaders
- [x] Deferred rendering
- [x] PBR
- [x] Ray marched SDF rendering
- [x] Ray marched & ray-traced hard shadows
- [x] Ray marched soft shadows
- [x] Ray marched & ray-traced specular reflections
- [x] HDRI skybox
- [x] Post-processing
- [ ] (WIP) Ray marched & hardware-accelerated ray-traced glossy reflections from screen-space roughness

## Platforms
**Hardware:**

A GPU with hardware ray tracing support is required.

**Operating systems:**

Currently only tested on Windows.

## Dependencies
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) (Installation required)
- [TGA](https://github.com/Estard/TGA)
- [GLFW](https://www.glfw.org/) 
- [GLM](https://github.com/g-truc/glm) 
- [stb single-file public domain libraries ](https://github.com/nothings/stb)
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)

## Getting started
### Installation and building
1) Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
2) Clone the repository with its dependencies using
  ```git clone --recursive https://github.com/alpcihan/hybrid```.
3) Use CMake to build the project.
### Examples

  You can find example projects under the [examples](./examples) folder.
  
  - Set the CMake variable ```HYBRID_EXAMPLES_ENABLED``` to ```ON``` to build the examples and download the associated [assets](https://github.com/alpcihan/hybrid-examples-assets). Note: it is ```ON``` by default.

#### Example: fractal_flythrough

An example [demo](./examples/fractal_flythrough) comprising a static fractal and a user-controllable spaceship model navigating in a 3rd person view within an abstract environment. The spaceship is rendered using rasterization, while the fractal is rendered using ray marching.

**Controls:**

- **W, A, S, D, Shift, and Space** to move the spaceship.
- **Up, Down, Left, Right** to rotate the spaceship.

## License
The project is licensed under [MIT license](./LICENSE).
