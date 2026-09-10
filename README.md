# MyGameEngine
My own standard C++ Game Engine created on SDL3 applying classic game and 
programming patterns (Component/GameObject, Command, Observer, State, Flyweight, Service Locator, Dirty Flag, Singleton)

## Repository layout
 
```
GameEngine/                 (root)
├── CMakeLists.txt          top-level: pulls in the engine, then a game
├── GameEngine/
│   ├── CMakeLists.txt      dependency fetching (SDL3, SDL3_ttf, glm, ImGui)
│   │                       + ge_apply_warnings / ge_install_runtime helpers
│   └── source/             engine source (gameEngine_lib)
│       ├── CMakeLists.txt  the engine's own source file list
│       ├── Components/
│       ├── Commands/
│       └── Services/
├── ProjectName/             <-- the starter template. Copy this to make a
│                                new game; see "Starting a new project" below
└── resources/               assets copied next to the exe at build time
```

Every game lives as its own top-level folder (sibling to `ProjectName/`)
with its own `CMakeLists.txt` and links against `gameEngine_lib`.

## Building Windows version

Either
- Open the root folder in Visual Studio 2026; this will be recognized as a cmake project.
  
Or
- Install CMake 
- Install CMake and CMake Tools extensions in Visual Code
- Open the root folder in Visual Code,  this will be recognized as a cmake project.

Or
- Use whatever editor you like :)

**Command line (any platform):**
 
```bash
cmake -B build -S .
cmake --build build --config Debug
```

### On OSX

On Mac you can use homebrew

    brew install cmake
    brew install emscripten
    brew install python

In a terminal on OSX, navigate to the root folder. Run this: 

    mkdir build_web
    cd build_web
    emcmake cmake .. -DCMAKE_OSX_ARCHITECTURES=""
    emmake make

## Starting a new project from the template

The template is meant to be copied, not built on top of directly. To start
`MyGame`:

1. **Copy the folder.** Duplicate `ProjectName/` to `MyGame/` (sibling of
   `ProjectName/`, same level as `GameEngine/`).
   
2. **Rename the target in `MyGame/CMakeLists.txt`.** Every occurrence of
   `projectName` needs to become your new target name (e.g. `myGame`).
   Currently that's in five places in this file:
   - `add_executable(projectName WIN32 ...)`
   - `set_property(... VS_STARTUP_PROJECT projectName)`
   - `set_target_properties(projectName PROPERTIES ...)`
   - both `add_custom_command(TARGET projectName POST_BUILD ...)` blocks

3. **Rename the class and namespace.** In `ProjectNameApp.h`/`.cpp`, rename
   `ProjectNameApp` and the `customProject` namespace to whatever fits your
   game, and update `Main.cpp`'s `#include` and the `customProject::ProjectNameApp app{};`
   line to match.

4. **Update the window title.** In `Main.cpp`,
   `ge::GameEngine::GetInstance().InitializeEngine(data_location, "New Project")`
   - change the string to your game's name.

5. **Add your new folder to the top-level `CMakeLists.txt`.** Add a line:
```cmake
   add_subdirectory(MyGame)
```
   next to the existing `add_subdirectory(ProjectName)` (or replace it, if
   you don't need the template kept around as a reference).

6. **Delete the starter code you don't need.** The FPS counter and F1
   resize binding in `Load()` exist only to prove the engine works
   on a fresh clone - replace them with your actual game setup.


## Adding assets
 
Anything placed in the top-level `resources/` folder is copied next to the
executable after every build ( `ge_install_runtime()` /
`ProjectName/CMakeLists.txt`'s post-build step )

Load assets through
`ge::ResourceManager::GetInstance()` - `LoadTexture`, `LoadFont`,
`LoadSpriteSheet` - using paths relative to `resources/`.

Resources need real types. For example, `TextComponent` (and therefore `FPSComponent` etc.) requires a real `.ttf/.otf` file to be loadable at the path passed to `LoadFont`.
