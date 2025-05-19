# TODO: Terminal Text-Mode 3D Renderer

A serious plan for building a robust ASCII-based 3D renderer.

---

## ✅ Core Renderer Engine

- [X] Refactor to SOA and pass indexes (Impossible difficulty)
- [ ] Interpolating lines (`VecLerp` or Bresenham)
- [ ] Z-sorting (use `cam.z` depth)
- [X] Triangle polygons
- [ ] Backface culling
- [ ] Assert math and rendering correctness
- [ ] Double Buffer
- [ ] Atomize logic into modules:
  - [ ] `camera.cpp / .hpp`
  - [X] `math3d.cpp / .hpp`
  - [X] `framebuffer.cpp`
  - [ ] `input.cpp`
- [ ] Add error-tolerant math (normalize, dot, cross edge cases)

---

## 🧪 Tooling & Dev UX

- [ ] Use `termios` + `fcntl` for non-blocking raw input
- [ ] Fuzzer + Simulator engine
- [X] `make debug` (`-g -O0 -DDEBUG`)
- [X] `make small` (`-Os -DDEBUG`)
- [X] `make run` (clear and run)
- [ ] Add keyboard shortcuts (WASD/orbit/pause)
- [ ] Test build on multiple terminals (Maybe add a makefile to launch multple GUI terms (maybe also installs them for you for developers based on distro or distro independent))

---

## 🧠 Testing & Documentation

- [X] Define project purpose (README)
- [ ] Document core pipeline steps (camera, projection, NDC)
- [ ] Comment important math (especially transforms)
- [ ] Unit test vector math
- [ ] Describe to a beginner how and *why* it works

---

## 🔄 Scene & Input System

- [ ] Support scene files (JSON / DSL)
    ```
    cube at 0 0 0
    camera orbit radius 5 speed 0.5
    ```
- [ ] Implement scene parser
- [ ] Support multiple objects
- [ ] Build simple `Renderable` abstraction

---

## ⚙️ System & Architecture

- [ ] Remove unnecessary dependencies
- [ ] Replace STL math with minimal in-house vector library
- [ ] Investigate multi-threaded rendering (rows or object batches)
- [ ] Consider structure-of-arrays layout for CPU cache locality

---

## 🌌 Stretch Goals

- [ ] Wireframe cubes (edges, lines)
- [ ] Lambertian lighting (dot product)
- [ ] ASCII shading using brightness table:
    `" .:-=+*#%@"`
- [ ] Triangle rasterization
- [ ] Backface culling
- [ ] Texture simulation (char patterns)
- [ ] GPU version (OpenGL / WebGPU port)
- [ ] Offline frame dump (to text files)

---

## 🧭 Navigation & Debug

- [ ] Add frame counter / FPS
- [ ] Keyboard toggles for debug overlays
- [ ] Save camera state to file

---

## 🧼 Cleanup

- [ ] Terminal reset on exit (cursor, echo, etc.)
- [ ] Exit key (`q`)
- [ ] Graceful error handling
