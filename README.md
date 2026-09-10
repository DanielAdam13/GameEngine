# MyGameEngine
My own standard C++ Game Engine created on SDL3 applying classic game and 
programming patterns (Component/GameObject, Command, Observer, State, Flyweight, Service Locator, Dirty Flag, Singleton)

## Links to Projects created on the Game Engine:

A game recreation of retro Bomberman

https://github.com/DanielAdam13/Prog4-GameEngine-Bomberman

A Jump Point Search research project, comparing with A*

https://github.com/DAE-GD-2025-2026/gameai-research-project-DanielAdam13


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


# Specific Engine Pattern Decision

### Component

    A Component is implemented "Not to reuse, but to be reused"

A Game Object OWNS a container of Components, not allowing any duplicates. The Game Object is the top-most level of abstraction in the game engine, it is the "main hub" for any methods/variables. Components, Commands, States all receive information via the Game Object target/owner. The feature of a Game Object not having any duplicate Components is very beneficial for a lot of reasons:
- It makes cross-component lookup very CONVENIENT - you just call GetComponent<T> (where T is the component type) from anywhere and the user is sure that they are receiving only a singular instance, no overcomplication in the regard of wondering "which one".
- Observers are all built on the assumption that only one Component of a type exists on a Game Object. If it didn't, the event logic would become entangled and complex.
- State coherence - if a Game Object has multiple Health Components, what does that even mean?

### Command

    Specify intent, not behavior
The Command Pattern is entirely separate from the Components. A Command will receive input and change values on the target Game Object but then a Component can do its own logic despite the command, making them, overall, a separate abstraction. The command's job is to encapsulate / "wrap" a function call and that's their main benefit. They do a very specific thing while being purely a call abstraction. 

Implemented in a clean layered design:
- Base abstraction (Command) -> allows Execute() overriding
- Game Object targetting (GameObjectCommand) -> base for command targetting specific Game Objects. Stores a non-owning reference so commands can be rebound.
- Game-flow commands (SwitchToMainMenuCommand...) -> act on game-wide state, override directly from base Command.
- UI Commands (HoverSelectableCommand) -> delegate to methods on a state. All hover/selection logic lives on the state, not in the command.

### Observer

    Decopuling identity - a Subject notifies an Obsever in the same call stack as the event source
Notifying that something has happened and not caring about who listens is a big deal in this project's architecture. 
A bomb explosion does not care about what sound is played or what is created from it, it only tells the rest of the program that THIS has happened.

### State

    Abstract setup, teardown, "What's next" logic outside of the update loop. The machine is the dispatcher and applies pending transitions
Engine fully supports utikizing State Machines:

   - Create a Base State class
   - Expose OnEnter and OnExit pure virtual methods
   - Expose an OnUpdate method that returns a unique_ptr to the new state
   - Create a separate State Machine class and store the new state 
   returned from OnUpdate. From here you can apply the pending state switch pattern.


### Update Method, Game Loop

    Simulating a collection of objects, one frame at a time while caring about Destruction/Additiong mid-iteration
This is basically the core loop of everything. 
A Scene Manager is Updated once a frame -> Calls Update for every Scene -> Calls Update for every Game Object -> Calls Update for every Component.

### Flyweight

    The engine applies half of it - memory sharing between object instances, SDL handles shared state for performance
The whole premise behind the ResourceManager is all resources having a shared place of ownership but MORE IMPORTANTLY - reusability and caching.
Textures, Spite Sheets, Fonts all benefit greatly from this.

### Dirty Flag

    Deffering an expensive calculation - Transform
The Transform's Component core idea - Marking Dirty ONLY when a recalculation is needed, not every frame. This means Setting the position, rotation or scale all 
mark the flag and Getting them does the Recalculation. This applies recursively to every CHILD of a Game Object as well.

### Singleton

    Ensure a class has one instance and provide a global point of access to it
Used in classes like ResourceManager, Renderer, GameEngine, the CollisionSystem. It is ok to expose these classes to every point since they are too high-level and would be
annoying to pass as a parameter for every call site - this would become TOO tedious, 100+ places in my code so far.

### Service Locator

    Tries solving the singleton issue by providing a shared point of access to things that NEED one instance
Applied for the Sound System and Input Manager. Initialized with a Null version until a derived service is assigned.

### Type Object

    Build a dynamic data-based object from a static class-based variation
Insted of subclassing every variation, we expose one class for the live thing and another for what TYPE it is.
Exact mapping in code:
- Typed object: EnemyComponent (dynamic instance). Has health, position, state machine, movement...
- Type object: EnemyArchetype (shared per-type data). SpriteSheet, speed, detection radius...

### Subclass Sandbox

    Providing a "sandbox" of available operations in a base class. Dervied classes reuse them.

You can put reusable methods in the protected field on a base State class...