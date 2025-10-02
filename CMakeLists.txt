cmake_minimum_required(VERSION 3.21)
project(renderingInC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # <— good habit when linking static libs

# ---- SDL2 static build options ----
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)

# Add vendored SDL2 (submodule at external/SDL on SDL2 branch/tag)
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

# Put the exe in a predictable dir (optional but nice)
set_target_properties(renderingInC PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Copy assets into the run directory after build
add_custom_command(TARGET renderingInC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:renderingInC>/assets"
)


# SDL targets (names vary by generator/SDL version)
set(SDL_TARGETS "")
if(TARGET SDL2::SDL2-static)
    list(APPEND SDL_TARGETS SDL2::SDL2-static)
elseif(TARGET SDL2-static) # fallback
    list(APPEND SDL_TARGETS SDL2-static)
endif()

# SDL2main is required on some platforms (notably Windows)
if(TARGET SDL2::SDL2main)
    list(APPEND SDL_TARGETS SDL2::SDL2main)
elseif(TARGET SDL2main) # older naming fallback
    list(APPEND SDL_TARGETS SDL2main)
endif()

# Be explicit if something went wrong
if(SDL_TARGETS STREQUAL "")
    message(FATAL_ERROR "Could not find SDL2 static targets from external/SDL")
endif()

target_link_libraries(renderingInC PRIVATE ${SDL_TARGETS})

# (Optional) Windows MSVC: link static runtime for easier distribution
if(MSVC)
    foreach(flag_var
            CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_DEBUG   CMAKE_C_FLAGS_MINSIZEREL)
        if(DEFINED ${flag_var})
            string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# (Optional) macOS: prefer plain binary (not an app bundle)
if(APPLE)
    set_target_properties(renderingInC PROPERTIES MACOSX_BUNDLE FALSE)
endif()
