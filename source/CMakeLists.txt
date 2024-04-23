#
# MIT License
#
# Copyright (c) 2024 unique-ones
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

file(GLOB ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp"
)

# Engine definition
add_library(engine "${ENGINE_SOURCES}")
target_link_libraries(engine PUBLIC extern glfw glm::glm freetype harfbuzz)

# Project source files
file(GLOB PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Executable definition with the project sources
add_executable("${PROJECT_NAME}"
    "${PROJECT_SOURCES}"
)

# Fetch glfw and glm from GitHub
include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_Declare(freetype GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git)
FetchContent_Declare(harfbuzz GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git GIT_TAG "8.4.0")
FetchContent_MakeAvailable(glfw glm freetype harfbuzz)

# Optionally fetch imgui from GitHub
if (IMGUI_LIBRARY)
    FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG master)
    FetchContent_MakeAvailable(imgui)

    file(GLOB IMGUI_SOURCES
        "${imgui_SOURCE_DIR}/*.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    )

    add_library(imgui STATIC ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
        "${imgui_SOURCE_DIR}/examples"
    )
    target_link_libraries(imgui PRIVATE glfw)
    target_link_libraries(engine PUBLIC imgui)
endif()

# Link libraries with project executable
target_link_libraries("${PROJECT_NAME}" PUBLIC engine)
