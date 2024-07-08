# Example of cmake sdl2 vcpkg opengl glad emscripten

## Goal
Compile to windows, linux, mac and the web.

For emscripten targeting webgl 2 and opengl es 3.0.


## build
### Windows
Make sure vcpkg is installed. And that the environment variable VCPKG_ROOT="Path to vcpkg"
```
mkdir windows-build
cd windows-build
cmake .. 
cmake --build .
```

### Web
Make sure emscripten is installed. And that emcmake is in path
```
mkdir web-build
cd web-build
emcmake cmake ..
cmake --build .
```

### Linux
Make sure vcpkg is installed. And that the environment variable VCPKG_ROOT="Path to vcpkg"
```
mkdir linux-build
cd linux-build
cmake .. 
cmake --build .
```

### Mac os
```
Not tested yet
```
