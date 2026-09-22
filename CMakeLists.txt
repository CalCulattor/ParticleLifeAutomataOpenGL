cmake_minimum_required(VERSION 3.20)

project(ParticleLifeAutomataOpenGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

add_executable(OpenGLProject
    src/main.cpp
    external/glad/src/gl.c
)

target_include_directories(OpenGLProject PRIVATE
    external/glad/include
)

target_link_libraries(OpenGLProject PRIVATE
    glfw
)

if(WIN32)
    target_link_libraries(OpenGLProject PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(OpenGLProject PRIVATE
        "-framework OpenGL"
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(OpenGLProject PRIVATE OpenGL::GL)
endif()