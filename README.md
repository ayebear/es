# Entity System

An easy to use, high performance, C++14 "Entity Component System" library.

### Table Of Contents

* [Purpose](#purpose)
* [Important Features](#important-features)
* [Terms Explained](#terms-explained)
* [Build Steps](#build-steps)
* [Example Usage](#example-usage)
  * [Entities](#entities)
  * [Components](#components)
  * [Systems](#systems)
  * [Events](#events)
  * [Prototypes](#prototypes)
* [Author](#author)

## Purpose

Creating real-time applications (games, GUIs, and simulations) can be made easier using a data oriented design, which this library allows you to do. It allows for a completely different organizational structure to your code than conventional OOP. It helps to:

* Improve performance (cache efficiency and order of updates)
* Reduce dependencies (events and independent systems)
* Increase flexibility (modding support, prototypes, entity definitions loaded during runtime)

This library was inspired by the following amazing libraries:
* OCS: https://github.com/Lastresort92/OCS
* EntityX: https://github.com/alecthomas/entityx
* Anax: https://github.com/miguelmartin75/anax


## Important Features

No other entity system currently has **all** of these features:

* Can use components with their string names
  * Accessing/updating/removing/checking components by name is supported
  * Allows for loading from files
  * Allows for network synchronization
* Allows for a mix of OOP and DOD
  * Easy to work directly with entities
  * Don't have to use the world interface to access components
* Efficient component access
  * Components are stored in contiguous arrays, one for each type
* Heavily uses the "assign" idiom
  * Accessing or setting components will automatically add them first if they don't already exist
  * Same for getting entities from the world
* Supports transferring entities between worlds
* Simplified systems for updating components
  * Built-in queries reduce boilerplate code
* Improved prototypes
  * Less work to define, don't need sets
  * Supports inheritance to reduce redundancy
  * Not tied to a single world instance


## Terms Explained

* System: Updates components.
  * **User defined:** This is where your logic goes.
* World: A collection of entities.
* Entity: A collection of components.
* Component: A struct that holds some data.
  * **User defined:** No logic should be here.
* Prototype: A preloaded set of components for an entity.
  * **User defined:** You can create a file containing the prototype definitions.


## Build Steps

### Build ES library

##### Checkout the repository and submodules:

```
git clone https://github.com/ayebear/es.git & cd es
git submodule init
git submodule update
```

##### Build the library:

```
mkdir build & cd build
cmake ..
make
```

At this point, you should have the files libes.so and libes_s.a, which you can dynamically or statically link to your project.

### Include and link ES with your project

1. Clone the repository or add it as a submodule to your repository.
2. Include and link ES in your CMakeLists.txt file:

```cmake
# Root of ES repository is in "lib/es"
add_subdirectory(lib/es)

# Link ES library (dynamically)
target_link_libraries(your_project LINK_PUBLIC es)

# Link ES library (statically)
target_link_libraries(your_project LINK_PUBLIC es_s)
```

If you aren't using CMake, you will need to download and build [ConfigFile](https://github.com/ayebear/ConfigFile) as well, which will need to be included and linked to your project.


## Example Usage

### Entities

##### Create a world to hold entities:

```cpp
#include "es/world.h"

es::World world;
```

##### Create entities:

Creating an entity returns a handle with a unique ID. This handle can be used directly to access components, as shown in the Components section.

Create an empty entity, which can optionally be named:

```cpp
auto ent = world.create();
auto ent2 = world.create("name");
```

Create an entity from a prototype, which can also be named:

```cpp
auto ent = world.copy("Type");
auto ent2 = world.copy("Type", "name");

// Those are just syntactic sugar for this:
auto ent3 = World::prototypes.get("Type").clone(world, "name");
// (The prototypes are stored in a static World instance)
```

Note: Using an invalid prototype name will return a valid, but empty entity.

##### Access entities:

```cpp
auto entId = ent.getId();

// Will be invalid if the ID/name does not exist
auto ent = world[entId];
auto ent = world.get(entId);
auto ent = world.get("name");

// Created automatically if it doesn't exist
auto ent = world["name"];

// Check if an entity is valid
if (ent)
    ...
if (ent.valid())
    ...
```

##### Register entity names:

Bind a name to an existing entity:

```cpp
ent.setName("name");
```

Then, you can access this entity from the world by its name, instead of its ID.

You can also get the name later on:
```cpp
auto name = ent.getName();
```

##### Copy entities:

Note: Calling clone() on an invalid entity will return a valid, but empty entity, from the specified world.

Inside a single world:

```cpp
// ent2 is just another reference to the same entity!
auto ent2 = ent;

// Copies all components and makes a new entity without a name
auto ent3 = ent.clone();

// Copies all components and makes a new entity with a name
auto ent4 = ent.clone("newName");
```

Between multiple worlds:

```cpp
es::World world2;

// Without a name
auto ent5 = ent.clone(world2);

// With a name
auto ent6 = ent.clone(world2, "newName");
```

Note: ent5 and ent6 are part of world2.

##### Delete entities:

Delete the entities' components, and remove the entity from the world:

```cpp
ent.destroy();
```

After this, the entity is no longer valid and should not be used.


##### Query entities:

Note: Both components and entities can be created/modified/deleted inside of these loops without issue. If you need to do this, be aware that deleting an entity may not stop it from being processed in the loop. Make sure to check that the entity and all components are valid before using them, if you decide to delete anything.

Any new entities that are created inside of the loop are guaranteed not to be processed, until the next call to query().

Iterate through the entities with all of the specified component types:

```cpp
for (auto ent: world.query<Position, Velocity, Sprite>())
{
    auto pos = ent.get<Position>();
    auto vel = ent.get<Velocity>();
    pos->x += vel->x;
    pos->y += vel->y;
}
```

Iterate through the entities with all of the specified component names:

```cpp
for (auto ent: world.query("Position", "Velocity", "Sprite"))
{
    auto pos = ent.get("Position");
    auto vel = ent.get("Velocity");
    float x1, y1, x2, y2;
    es::unpack(*pos, x1, y1);
    es::unpack(*vel, x2, y2);
    x1 += x2;
    y1 += y2;
    *pos = es::pack(x1, y1);
}
```


### Components

#### Defining components

```cpp
#include "es/component.h"
#include "es/serialize.h"

struct Position: public es::Component
{
    static constexpr auto name = "Position";

    float x, y;

    Position(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};
```

#### Registering components

After your components are defined, you must register their types before using them. This internally sets up arrays, component names, and type indexes.

This should go in your initialization code, **before** using any entities with your components. It only needs to be called once, since the component names and types are static.

```cpp
#include "es/componentpool.h"

es::registerComponents<Position, Velocity, Sprite, Size, AABB, Gravity>();
```

#### Using components with entities

##### Create/update components:

If the component already exists, these will set the component, otherwise a new component will be created.

```cpp
// These can be chained like std::cout
ent << Position(10, 20);

// The efficient argument forwarding way
ent.assign<Position>(30, 40);

// Similar to operator<<
ent.assignFrom(Position(50, 60));
```

##### Accessing components:

By default, accessing components will return a special Handle object. This allows you to store the handles for long periods of time, and they still work properly even if the array reallocates, or if the components themselves are moved in memory.

Access components (will be created if they don't exist):

```cpp
// Get component handle of specified type
auto compHandle = ent.at<Position>();

// Get base component handle
auto baseCompHandle = ent.at("Position");

// Get component reference of specified type
auto& comp = ent.access<Position>();

// Get direct base component references
auto& baseComp = ent["Position"];
auto& baseComp2 = ent.access("Position");

// Can chain calls from the world
world["pos1"]["Position"] = "50 80";
```

Access components (won't create):

```cpp
auto baseComp = ent.get("Position");
auto comp = ent.get<Position>();

Position pos;
ent >> pos;
```

Use the handle to access members in a component:

```cpp
if (comp)
{
    comp->x = 5;
    (*comp).x = 5;
    comp.access().x = 5;
}
```

##### Deserialize:

```cpp
// With component name
ent << "Position 100 500";
ent.deserialize("Position 100 500")

// Separate component name
ent["Position"] = "100 500";
ent.at<Position>().load("100 500");
```

##### Serialize:

```cpp
// Get a string of a component
auto str = ent["Position"].save();

// Implict string cast
std::string posStr;
posStr = ent.at<Position>();
posStr = ent["Position"];
```

##### Serialize all components:

```cpp
// Returns a vector of strings
auto comps = ent.serialize();
```

#### Iterate through components

Sometimes, you don't need to update things at the entity level, and may want to directly iterate through the internal component arrays. This is much more cache efficient than querying for entities.

Note: Components don't store the ID of the entity that owns it, so if you need to lookup other components that are part of the same entity, you'll have to use the query() function explained above in the "Entities" section.

```cpp
// Update all position components by (2, 1)
for (auto& pos: world.getComponents<Position>())
{
    pos.x += 2;
    pos.y += 1;
}
```


### Systems

#### es::System

A simple base class with initialize() and update() methods. This is used by the SystemContainer class.

##### Example user-defined system:

```cpp
#include "es/system.h"

class MovementSystem: public es::System
{
public:
    void update(float dt)
    {
        for (auto ent: world->query())
        {
            auto pos = ent.get<Position>();
            auto vel = ent.get<Velocity>();
            if (pos && vel)
            {
                pos->x += vel->x * dt;
                pos->y += vel->y * dt;
            }
        }
    }
};
```

#### es::SystemContainer

This class stores instances of es::System classes, and can call initialize/update on all/any of them. Both initializeAll() and updateAll() will call the methods in the order the systems were added.

##### Add systems to a container

```cpp
es::World world;
es::SystemContainer systems(world);
systems.add<MovementSystem>();
systems.add<RenderSystem>(window);
```

##### Initialize and update the systems

```cpp
// Call this before your program starts
systems.initializeAll();

// Call this in a loop
systems.updateAll(dt);

// Also supports updating single systems by type:
systems.update<MovementSystem>(dt);
```

### Events

Events are used to allow systems to communicate without depending on each other. The event system provided is completely optional, you may use your own if you wish.

#### es::Events

This is a simple class that provides static functions for sending global events, separated by type.

Internal storage: The events are directly stored in a deque for each type, which should be faster than vector for this use case. Deque's do not reallocate when more space is needed, which speeds up sending events. Even though deque is less cache-efficient than vector, the difference will be negligible, since normally only one piece of code will iterate through the events on the receiving end, before it gets cleared.

##### Defining events

Any type can be used as an event. Example:

```cpp
struct MyEvent
{
    std::string text;
    int number;
};
```

##### Sending events

Events are sent to the queue of that particular type:

```cpp
// C++11 brace-init style (does not require a constructor)
es::Events::send(MyEvent{"Some text", 20});

// Emplace the event (requires a parameterized constructor)
es::Events::send<MyEvent>("Some text", 20);

// Send an existing event
MyEvent event;
...
es::Events::send(event);
```

##### Receiving events

The safest and most efficient way to receive events is to iterate through them:

```cpp
for (auto& event: es::Events::get<MyEvent>())
    doSomethingWithEvent(event);
```

##### Clearing events

The events still exist in memory even after iterating through them. This is so multiple systems can access the same events. Because of this, you must clear the events each loop, so events aren't processed more than once.

To clear all of the events:

```cpp
es::Events::clearAll();
```

It seems simple to just call this in your main loop, but what if your systems aren't in the correct order? You'll have events that are never received, because they will be cleared before anything receives them. Here are some solutions:
  * Re-order your systems so all events are received properly (isn't always easy or possible)
  * Run multiple passes for updating things (this has a lot of issues)
  * Manually clear event types (tedious, but works the best)

To clear specific event types:

```cpp
es::Events::clear<MyEvent>();
```

This will clear all of the events of type "MyEvent". Here are some tips on where to call this in your code:

* One sender, multiple receivers:
  * Call clear right **before** the events are **sent**. This way, even if a system is a frame behind, the event will still be received.
* Multiple senders, one receiver:
  * Call clear right **after** all of the events of that type have been **received**.


### Prototypes

Prototypes can be defined in the [ConfigFile](https://github.com/ayebear/ConfigFile) format, and loaded from various sources such as a file, string, or a network.

* Component names must be properly defined/bound first.
* Prototypes and their names are static, so they don't need to be reloaded across worlds.
* Multiple inheritance is fully supported.
  * Loops and the diamond problem are solved by ignoring entity names already loaded into each entity.
  * As the same components are encountered, they are overridden with the latest components.

##### Example prototypes file:

Filename: "entities.cfg"

```dosini
[Example]
Component = "parameter1 parameter2 parameter3"
ComponentFlag = ""

[SomeEntity]
Position = "200 200"
Velocity = "50 50"
Size = "128 128"

[SubEntity: SomeEntity, Example]
Description = "An entity with all of the components of SomeEntity and Example"

[SubEntity2: SomeEntity]
Size = "64 64"
```

##### Code to load the prototypes file:

```cpp
es::loadPrototypes("entities.cfg");
```


## Author

Eric Hebert
