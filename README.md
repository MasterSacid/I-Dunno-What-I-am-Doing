# 3D Software Renderer in C (SDL2) 🎨

A from-scratch, CPU-based 3D renderer written in C11 using SDL2 for windowing and presenting a framebuffer. It loads Wavefront OBJ meshes with PNG textures, performs vertex transforms, backface culling, frustum clipping, per-vertex directional lighting, perspective-correct textured rasterization with a Z-buffer, and includes a simple water/shimmer effect.

Project name in CMake: `renderingInC`.

---

## Features ✨

* C11, single executable, cross-platform via SDL2.
* Wavefront OBJ mesh loading with per-mesh PNG textures (via a minimal PNG decoder `upng`).
* CPU rasterizer with Z-buffering and frustum clipping.
* Perspective/camera system with free-fly FPS controls and mouselook.
* Directional light with ambient term; per-vertex intensities.
* Gouraud shading for smooth lighting across surfaces.
* Multiple render modes you can toggle at runtime:

  * Wireframe only
  * Wireframe + vertices
  * Solid filled triangles
  * Solid + wireframe overlay
  * Textured
  * Textured + wireframe overlay
* Backface culling toggle.
* Water surface effect (simple shimmer) available per-mesh.
* Fullscreen by default on macOS; internal color/z buffer sized to half the display resolution for performance.

---

## Controls 🎮

From `src/main.c` and `src/display.[ch]`:

* Movement: W/A/S/D (planar), Space (up), Left/Right Ctrl (down)
* Mouse: Look around (relative mouse mode)
* Render mode:

  * 1: Wireframe + vertices
  * 2: Wireframe
  * 3: Filled
  * 4: Filled + wireframe
  * 5: Textured
  * 6: Textured + wireframe
* Culling:

  * C: Backface culling ON
  * X: Culling OFF
* ESC: Quit

Notes:

* Relative mouse mode is enabled on startup (mouse is captured and hidden). Press ESC to exit the app; Cmd+Tab to switch focus on macOS.
* Default render mode is Textured with backface culling.

---

## Build and Run 🛠️

This project is built with **CMake (3.21+)**. SDL2 is vendored as a **Git submodule** under `external/SDL`, so you don’t need to install SDL2 manually — it will be built along with the project.

### Setup Instructions (Linux/macOS)

1. Clone the repository (including submodules):

   ```bash
   git clone --recursive https://github.com/MasterSacid/C-renderer
   cd 3drenderer
   ```

   If you already cloned without `--recursive`, run:

   ```bash
   git submodule update --init --recursive
   ```

2. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

3. Configure the project:

   ```bash
   cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
   ```

4. Build the project:

   ```bash
   cmake --build . -j
   ```

5. Run the application:

   ```bash
   ./renderingInC
   ```

---

### Setup Instructions (Windows with MinGW/MSYS2)



1. Install [MSYS2](https://www.msys2.org/).

2. Open the **MSYS2 MinGW64 terminal** and install the toolchain:

   ```bash
   pacman -S --needed base-devel git cmake ninja mingw-w64-x86_64-toolchain
   ```

3. Clone the repository (with submodules):

   ```bash
   git clone --recursive https://github.com/MasterSacid/C-renderer
   cd 3drenderer
   ```

4. Configure with CMake:

   ```bash
   cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
   ```

5. Build:

   ```bash
   cmake --build build
   ```

6. Run the program:

   ```bash
   ./build/renderingInC.exe
   ```


---

## Assets 📂

This repository includes sample OBJ and PNG textures under the `assets/` directory.

“At build time, CMake copies the `assets/` folder next to the executable. You can place your own OBJ/PNG files there and load them, but keep meshes under ~10,000 triangles. When exporting from Blender, make sure to enable the *Triangulated Mesh* option.”



### Default Meshes

By default, `setup()` in `src/main.c` loads:

* Three instances of `assets/Car 01/Car.obj` with different textures (`car.png`, `car_red.png`, `car_blue.png`).
* A surface mesh `assets/surface.obj` with `assets/pool2.png`, using the water effect.
* (Optional) A cube mesh from `assets/cube.obj` (already present, commented out in the code).

### Example

```c
loadMesh("assets/Car 01/Car.obj", "assets/Car 01/car.png",
         vec3New(1, 1, 1),
         vec3New(-4, 0, 8),
         vec3New(M_PI/3.0, M_PI/3.0, 0),
         NONE);
```

---

## Lighting and Shading 💡

The renderer uses a simple directional light model with ambient lighting. The light direction (`sunRaysDir`) is defined in `setup()` as:

```c
vec3_t sunRaysDir = {0.0f, 3.0f, 0.0f};
vec3Normalize(&sunRaysDir);
light.direction = vec3Multiply(sunRaysDir, 1.0f);
```

You can tweak the `sunRaysDir` vector to adjust the light's direction and intensity.

Gouraud shading is used to interpolate lighting across vertices, providing smooth shading for surfaces.

---

## Configuration and Tweaks ⚙️

* Initial render/cull modes are set in `setup()` (`src/main.c`).
* Target FPS is 120 (`display.h`: `FPS`, `FRAME_TARGET_TIME`).
* Fullscreen is enabled by default
* **Resolution Adjustment**: In `display.c`, the `windowWidth` and `windowHeight` are set to half the fullscreen resolution by default:

  ```c
  windowWidth = fullscreenWidth / 2;
  windowHeight = fullscreenHeight / 2;
  ```

  You can modify the divisor (e.g., replace `2` with another value) to adjust the resolution to your needs:

  * Setting the divisor closer to `1` (e.g., `1`) will increase the resolution, making the visuals sharper.
  * Using larger values (e.g., `3`, `4`, etc.) will create a more pixelated, nostalgic look.

---

## Project Structure 🗂️

```
3drenderer/
├── src/                    # Source code for the renderer
│   ├── array.[ch]          # Dynamic array helper
│   ├── camera.[ch]         # Free-fly camera and mouselook
│   ├── clipping.[ch]       # Frustum planes and triangle clipping
│   ├── display.[ch]        # Windowing, color/z buffer, draw calls
│   ├── light.[ch]          # Simple directional light
│   ├── main.c              # App loop and pipeline orchestration
│   ├── matrix.[ch]         # Matrix math primitives
│   ├── mesh.[ch]           # OBJ/PNG loading, mesh data
│   ├── swap.[ch]           # Utility for swapping values
│   ├── texture.[ch]        # Texture coordinate handling
│   ├── triangle.[ch]       # Triangle structures and rasterization helpers
│   ├── upng.[ch]           # Tiny PNG loader
│   ├── vector.[ch]         # Vector math primitives
│   └── water.[ch]          # Water/shimmer effect parameters and helpers
├── assets/                 # Sample OBJ/PNG assets (copied next to binary at build time)
│   ├── Car 01/             # Car model and textures
│   ├── cube.obj            # Cube model
│   ├── surface.obj         # Surface model
│   └── pool2.png           # Texture for the surface
├── external/SDL/           # SDL2 submodule (vendored library)
├── CMakeLists.txt          # Build configuration
└── README.md               # Project documentation
```

---

## Screenshots 📸


* Main scene

  ![Main scene](docs/mainscene.png)

* Wireframe mode

  ![Wireframe mode](docs/wireframe.png)

* Water effect 

  ![Water effect](docs/water.png)
* Shading
![Shading](docs/shading.png)

---

## Resources and Acknowledgments 🙏

### Resources

* **Water Animation**: The water animation effect in this project was inspired by techniques described in the book *GPU Gems*.
* **Gouraud Shading**: I referred to [this StackExchange post](https://computergraphics.stackexchange.com/questions/10845/how-exactly-does-gouraud-shading-apply-color-across-a-polygon-does-it-do-it-by) to understand how Gouraud shading applies color across a polygon.

### Acknowledgments

* I followed Gustavo Prezzi's lectures for core rendering maths and rendering pipeline.
