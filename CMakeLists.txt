cmake_minimum_required(VERSION 3.31)
project(renderingInC C)

set(CMAKE_C_STANDARD 11)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)




add_executable(renderingInC src/main.c
        src/display.h
        src/display.c
        src/vector.c
        src/vector.h
        src/mesh.c
        src/mesh.h
        src/triangle.c
        src/triangle.h
        src/array.h
        src/array.c
)


target_include_directories(renderingInC PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(renderingInC PRIVATE ${SDL2_LINK_LIBRARIES})