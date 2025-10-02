cmake_minimum_required(VERSION 3.21)  # 3.21+ for good FetchContent UX
project(renderingInC C)

# ---- Basics ---------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to a sensible build type if none given
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# On MSVC, use the static runtime in Release for easier distribution (optional)
if(MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()
endif()

# ---- Dependencies: SDL2 ---------------------------------------------------
# Option 1 (fast path): use system-installed SDL2 if present
option(USE_SYSTEM_SDL2 "Prefer system-installed SDL2 if available" ON)
set(SDL2_REQUIRED_VERSION 2.0)

set(SDL_TARGETS_OK FALSE)

if(USE_SYSTEM_SDL2)
    # Try Config mode first (vcpkg/conan/homebrew cmake packages), then Module mode
    find_package(SDL2 ${SDL2_REQUIRED_VERSION} QUIET CONFIG)
    if(NOT SDL2_FOUND)
        find_package(SDL2 ${SDL2_REQUIRED_VERSION} QUIET) # module mode (FindSDL2.cmake)
    endif()

    if(SDL2_FOUND)
        set(SDL_TARGETS_OK TRUE)
    endif()
endif()

# Option 2: FetchContent fallback (or vendor as submodule)
if(NOT SDL_TARGETS_OK)
    include(FetchContent)
    # If you commit SDL2 as a submodule at external/SDL (recommended for pinning),
    # replace the GIT_ lines with SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL"
    FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG        release-2.28.5  # pin a known working SDL2 tag
            # For SDL3 you'd switch repo/tag AND update code accordingly
    )
    FetchContent_MakeAvailable(SDL2)
    set(SDL_TARGETS_OK TRUE)
endif()

if(NOT SDL_TARGETS_OK)
    message(FATAL_ERROR "SDL2 could not be found or fetched.")
endif()

# SDL naming differs depending on how it was found/built:
# Prefer imported targets if they exist.
set(SDL_LINK_TARGETS "")
if(TARGET SDL2::SDL2)
    list(APPEND SDL_LINK_TARGETS SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    list(APPEND SDL_LINK_TARGETS SDL2::SDL2-static)
endif()

# Some platforms also need SDL2main (esp. Windows)
if(TARGET SDL2::SDL2main)
    list(APPEND SDL_LINK_TARGETS SDL2::SDL2main)
endif()

if(SDL_LINK_TARGETS STREQUAL "")
    message(FATAL_ERROR "Could not determine SDL2 targets to link against.")
endif()

# ---- Your executable ------------------------------------------------------
add_executable(renderingInC
        src/main.c
        src/display.h src/display.c
        src/vector.c src/vector.h
        src/mesh.c src/mesh.h
        src/triangle.c src/triangle.h
        src/array.c src/array.h
        src/matrix.c src/matrix.h
        src/light.c src/light.h
        src/texture.c src/texture.h
        src/swap.c src/swap.h
        src/upng.c src/upng.h
        src/camera.c src/camera.h
        src/clipping.c src/clipping.h
        src/water.c src/water.h
)

target_link_libraries(renderingInC PRIVATE ${SDL_LINK_TARGETS})

# If you need the SDL include dirs explicitly (usually not needed with targets)
# but this helps when FindSDL2.cmake only defines SDL2_INCLUDE_DIRS:
if(DEFINED SDL2_INCLUDE_DIRS)
    target_include_directories(renderingInC PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# macOS app bundle niceties (optional)
if(APPLE)
    # Ensure it runs in a bundle or set @rpath correctly (optional):
    set_target_properties(renderingInC PROPERTIES
            MACOSX_BUNDLE FALSE
    )
endif()
