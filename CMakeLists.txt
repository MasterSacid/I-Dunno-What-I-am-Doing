cmake_minimum_required(VERSION 3.21)
project(renderingInC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---- SDL2 static build options ----
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)

# Add vendored SDL
add_subdirectory(external/SDL)

# ---- Your executable ----
add_executable(renderingInC
        src/main.c
        src/display.c src/display.h
        src/vector.c  src/vector.h
        src/mesh.c    src/mesh.h
        src/triangle.c src/triangle.h
        src/array.c   src/array.h
        src/matrix.c  src/matrix.h
        src/light.c   src/light.h
        src/texture.c src/texture.h
        src/swap.c    src/swap.h
        src/upng.c    src/upng.h
        src/camera.c  src/camera.h
        src/clipping.c src/clipping.h
        src/water.c   src/water.h
)

# SDL targets (depends on SDL version/platform)
set(SDL_TARGETS "")
if(TARGET SDL2::SDL2-static)
    list(APPEND SDL_TARGETS SDL2::SDL2-static)
elseif(TARGET SDL2-static) # fallback
    list(APPEND SDL_TARGETS SDL2-static)
endif()

if(TARGET SDL2::SDL2main)
    list(APPEND SDL_TARGETS SDL2::SDL2main)
endif()

target_link_libraries(renderingInC PRIVATE ${SDL_TARGETS})
