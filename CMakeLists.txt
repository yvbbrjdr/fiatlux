cmake_minimum_required(VERSION 3.1.3)
project(fiatlux)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Wextra)

add_executable(fiatlux
    src/common/sampler/sampler_2d.cpp
    src/common/sampler/sampler_hemisphere.cpp
    src/common/sampler/sampler.cpp
    src/gui/gui.cpp
    src/image/image.cpp
    src/image/png.cpp
    src/image/ppm.cpp
    src/main.cpp
    src/pathtracer/pathtracer.cpp
    src/scene/accel/bbox.cpp
    src/scene/accel/bvh.cpp
    src/scene/bsdf.cpp
    src/scene/camera.cpp
    src/scene/scene.cpp
    src/scene/texture.cpp
    src/scene/triangle.cpp
    src/thirdparty/lodepng/lodepng.cpp
    src/thirdparty/tinygltf.cpp
)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/thirdparty/lodepng)
include_directories(${CMAKE_SOURCE_DIR}/src/thirdparty/tinygltf)
add_definitions(
    -DLODEPNG_NO_COMPILE_DECODER
    -DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
    -Wno-ignored-qualifiers
)
find_package(Threads REQUIRED)
target_link_libraries(fiatlux ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fiatlux SDL2)
