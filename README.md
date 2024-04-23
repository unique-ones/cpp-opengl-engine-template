# C++ and OpenGL engine template

This repository serves as a template for starting computer graphics development in C++.

![opengl](https://github.com/unique-ones/cpp-opengl-template/assets/45200003/a9cb589f-d5fc-464a-9055-9cce4fa33e23)

## Prerequisites

In order to use this template, you must have a few things installed:

 - [CMake](https://cmake.org)
 - [LLVM](https://llvm.org)
 - [Ninja](https://ninja-build.org)
 - [VSCode](https://code.visualstudio.com)

Make sure that all of the above are installed correctly and that they are set in your `PATH` environment variable. This template is specifically tailored to development with VSCode.

## Getting Started

### 1. Clone this repository:

```bash
git clone https://github.com/unique-ones/cpp-opengl-engine-template.git
```

### 2. Rename the cloned repository to a name of your liking:

```bash
mv cpp-opengl-engine-template your-cool-project-name
```

### 3. Navigate into the cloned repository:

```bash
cd your-cool-project-name
```

### 4. Open VSCode

```bash
code .
```

This should fire up a new instance of VSCode. On the bottom right, you should get a notification that sounds something like this: _This workspace has extension recommendations_.

The project is preconfigured to include important extensions such as support for CMake, debugging and code completion (clangd). Click on _Install All_.

### 5. Rename the project in the `CMakeLists.txt` file

You should see a line that looks something like this:

```CMake
project(cpp-opengl-engine-template LANGUAGES C CXX)
```

Change this to the name of your project:

```CMake
project(your-cool-project-name LANGUAGES C CXX)
```

## Building

### VSCode

In order to build the project using VSCode, you must first configure the CMake project:

1. Press `F1` to open the command palette
2. Type in _CMake: Configure_
3. Press `Enter`

You will probably get asked what preset you want to choose, you can choose from
 - Debug: `<os>-64-debug`
 - Release: `<os>-64-release`

Then you can build the project:

1. Press `F1` to open the command palette
2. Type in _CMake: Build_
3. Press `Enter`

### Command Line

In order to build the project using the commandline, you must first configure the CMake project. There are different presets to choose from, you can list them using:

```bash
cmake --list-presets
```

There will probably be two presets to choose from:
 - Debug: `<os>-64-debug`
 - Release: `<os>-64-release`

You can then go on and choose a preset using the following command:

```bash
cmake --preset=<preset-name>
```

After this, you can build the project. For building, there are also different presets to choose from. It must be noted that the build preset **must** match with the configure preset. You can see all possible build presets using the following command:

```bash
cmake --build --list-presets
```

Now you can build the project using a preset of your choice:
```bash
cmake --build --preset=<preset-name>
```

## Development

### Adding Source Files

The CMake project is configured in a way, such that if you add source files to the `source` folder, they are automatically picked up and compiled if you reconfigure CMake. It must be noted that this only works if your source files end with `.cpp` and your header files end with `.h`.

### Handling Of Assets

The project's assets should be contained in the `assets` folder. This folder is automatically copied into the build folder whenever the CMake project is reconfigured. That implies that in order to use newly added assets, you must reconfigure the CMake project.

### The Engine

The project ships with a basic 2D Renderer that supports batch and text rendering. See the provided `main.cpp` for an example on how to use this engine.

### Optional Libraries

Currently, the only optional library that is included with this template is [Dear ImGui](https://github.com/ocornut/imgui). To use it in your project, you simple have to locate the following line in the root `CMakeLists.txt`:

```CMake
set(IMGUI_LIBRARY OFF)
```

and change it as follows:

```CMake
set(IMGUI_LIBRARY ON)
```
