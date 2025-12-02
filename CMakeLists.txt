
cmake_minimum_required(VERSION 3.28)
project(minecraft_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# OpenGL
find_package(OpenGL REQUIRED)

# Use GLAD for OpenGL function loading (FetchContent)
include(FetchContent)
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLFW 
include(FetchContent)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

add_executable(minecraft_cpp
    src/main.cpp
    src/core/Window.cpp
    src/core/Renderer.cpp
    src/core/Camera.cpp
)

target_link_libraries(minecraft_cpp glfw OpenGL::GL glm glad)
