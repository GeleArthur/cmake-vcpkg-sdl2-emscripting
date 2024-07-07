# Example of cmake sdl2 vcpkg opengl glad emscripten

## Goal
Compile to windows, linux, mac and the web.

For emscripten targeting webgl 2 and opengl es 3.0.


## build
### Windows
```
mkdir windows-build
cd windows-build
cmake ..
cmake --build .
```

### Web
Make sure emscripten is installed
```
mkdir web-build
cd web-build
emcmake cmake ..
cmake --build .
```

### Linux
```
Not tested yet
```

### Mac os
```
Not tested yet
```
