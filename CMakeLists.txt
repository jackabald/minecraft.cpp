cmake_minimum_required(VERSION 3.14)
project(minecraft_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Fetch GLFW from GitHub
include(FetchContent)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

add_executable(minecraft_cpp
    src/main.cpp
    src/core/Window.cpp
)

target_link_libraries(minecraft_cpp glfw OpenGL::GL)
