# MyGameEngine
My own standard C++ Game Engine created on SDL3 applying classic game and programming patterns.

The goal was to create a reusable game engine rather than simply a collection of rendering and input functions. The engine provides a foundation for building games, with a Game Object / Component architecture, scene management, rendering, collision detection, resource management, input handling and more.

The engine is built around the idea that systems should have their own responsibilities while remaining easy to combine. Components implement behavior, Commands specify intent, and the engine handles the underlying game loop and object lifecycle.

## Playable Result

A recreation of the retro game Bomberman, built using this engine.

https://danieladam13.github.io/Prog4-GameEngine-Bomberman/

Source Code

https://github.com/DanielAdam13/Prog4-GameEngine-Bomberman

The Bomberman project demonstrates how the engine can be reused to build an actual game, including player movement, bomb placement, explosions, enemy AI, powerups, level generation, menus and game-state transitions.

The game uses the engine's GameObject / Component architecture, Command and Observer patterns, State Machines, collision system, resource manager and scene management.

Another project built using the engine is a Jump Point Search research project comparing JPS with A*.

https://github.com/DAE-GD-2025-2026/gameai-research-project-DanielAdam13

## Architecture

## Technical Highlights

### GameObject / Component

A Component is implemented "Not to reuse, but to be reused".

A Game Object owns a container of Components, without allowing duplicates of a component type.

This makes it easy to combine functionality without creating a deep inheritance hierarchy. A Game Object can have a Transform, Renderer-related Components, Colliders and gameplay-specific Components, all working together.

Component lookup uses compile-time type IDs and indexed storage, making GetComponent<T>() straightforward and avoiding the need to search through a list of Components.

### Hierarchical Transforms / Dirty Flag

The Transform Component supports parent-child relationships, allowing objects to inherit transformations from their parents.

Local transformation matrices are combined with the parent's world matrix to calculate the final world transformation.

The Dirty Flag pattern defers this calculation until the world matrix is requested. Changing a local position, rotation or scale marks the Transform as dirty, recursively invalidating its descendants as well.

This avoids recalculating world matrices when nothing has changed.

### Object Lifecycle / Update Method

The Scene updates its Game Objects, which in turn update their Components.

Adding or removing objects during iteration can invalidate iterators and cause undefined behavior. Instead, additions are stored as pending additions and removals are handled through a Mark For Delete mechanism.

This allows gameplay logic to create and destroy objects without directly modifying the collection being iterated over.

### Command / Observer

Commands specify intent, Components implement behavior.

The Command pattern separates input handling from gameplay logic. The Input Manager executes Commands, which can target individual Game Objects or perform game-wide actions.

The Observer pattern separates event sources from the systems reacting to them. A bomb explosion does not need to know what sound should play or which objects should be created. It simply notifies its Observers that the explosion happened.

These patterns are used together in the Bomberman game to keep gameplay systems separate and extensible.

### Resource Management / Flyweight

The ResourceManager provides a shared place of ownership and caching for textures, sprite sheets and fonts.

Game Objects can reuse loaded resources rather than loading duplicate copies for every instance.

Resources are managed through RAII, with std::unique_ptr handling ownership and non-owning references used where appropriate.

### State Machines

The engine provides the foundation for implementing State Machines, while the Bomberman game demonstrates their actual use.

The game uses a Game State Machine to manage transitions between menus, gameplay and game-over states. Enemy AI uses separate States for behaviors such as wandering and chasing the player.
State transitions are deferred and applied at a controlled point in the update loop.

## Important Engine Decisions

### Why Components instead of inheritance?

Game Objects are composed from Components rather than requiring a separate subclass for every combination of behavior.

This keeps functionality reusable and avoids deep inheritance hierarchies. It also makes cross-component communication convenient through GetComponent<T>().

### Why defer object destruction and additions?

Modifying a collection while iterating over it can invalidate iterators and references.

Pending additions and deferred deletion provide a controlled point for modifying the Scene's Game Object collection, keeping the update loop safe.

### Why use a fixed-size array for Components?

Component types have unique type IDs that can be used as direct indices into the Game Object's Component container.

This trades flexibility for straightforward, fast lookup. Since a Game Object cannot contain multiple Components of the same type, lookup is unambiguous.

### Why cache resources?

Textures, sprite sheets and fonts are often shared by many Game Objects.

The ResourceManager owns these resources and reuses them, avoiding unnecessary duplicate loads and giving their lifetime a central owner.

### Why Commands instead of putting gameplay logic in the Input Manager?

The Input Manager should handle input, not understand the behavior of every game mechanic.

Commands encapsulate actions and can be rebound or replaced without changing the input system. Components remain responsible for their own behavior, keeping the two abstractions separate.

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


## Github Actions

This project uses GitHub Actions to automate builds.

CMake workflow - Builds Debug and Release configurations for Windows, checking that the project compiles on that platform.

Emscripten workflow - Builds the web version and publishes it through GitHub Pages.

The published web version follows this URL format:

https://<username>.github.io/<repository>/