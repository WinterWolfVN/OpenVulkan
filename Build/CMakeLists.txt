cmake_minimum_required(VERSION 3.10)

project(MVulkan)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    mvulkan
    SHARED

    ../MVulkan/JNI/native-lib.cpp

    ../Shared/GLES/gl_context.cpp
    ../Shared/GLES/gl_shader.cpp
    ../Shared/GLES/gl_buffer.cpp
)

find_library(
    log-lib
    log
)

find_library(
    android-lib
    android
)

target_include_directories(
    mvulkan

    PRIVATE

    ../Shared
    ../Shared/Vulkan
    ../Shared/GLES
    ../MVulkan
)

target_link_libraries(
    mvulkan

    EGL
    GLESv3

    ${log-lib}
    ${android-lib}
)
