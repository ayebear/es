# Entity System

An easy to use, high performance, C++14 "Entity Component System" library.

Note: Depending on how much [OCS](https://github.com/Lastresort92/OCS) gets updated, this library may not be needed.


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

With all of the existing entity systems, here are the reasons I needed/wanted this one:

* Can access/update components with strings
  * Allows for loading from files
  * Allows for network synchronization
* Allows for a mix of OOP and DOD
  * Easy to work directly with entities
  * Don't have to use the world interface to access components
* Efficient component access
  * Components are stored in contiguous arrays, one for each type
* Heavily uses the "assign" idiom
  * Accessing or setting components will automatically add them first if they don't already exist
* Simplified systems for updating components
  * Built-in, cache efficient filters reduce boilerplate code
* Improved prototypes
  * Less work to define, don't need sets
  * Supports inheritance to reduce redundancy
  * Not tied to a single world instance


## Terms Explained

* System: Updates components. This is where your logic goes.
* World: A collection of entities.
* Entity: A collection of components.
* Component: A struct that holds some data. No logic should be here.
* Prototype: A preloaded set of components for an entity, can be defined in a file.


## Prototypes

Prototypes can be defined in the [ConfigFile](https://github.com/ayebear/ConfigFile) format, and loaded from various sources such as a file, string, or a network.

* Component names must be properly defined/bound first.
* Prototypes and their names are global, so they don't need to be reloaded across worlds.
* Multiple inheritance is fully supported. Loops and the diamond problem are solved by ignoring entity names already loaded into each entity.

#### Example prototypes file:

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

#### Code to load the prototypes file:

```cpp
es::loadPrototypes("entities.cfg");
```


## Example Usage

Note: Some of these features aren't fully designed/implemented yet, and may change in the future.

### Entities

##### Create a world to hold entities:

```cpp
es::World world;
```

##### Create entities:

```cpp
auto ent = world.create(); // Empty, unnamed entity
auto ent = world.create("name"); // Empty entity with name
auto ent = world.clone("Type"); // From prototype, with no name
auto ent = world.clone("Type", "name"); // From prototype, register name
auto ent = world["name"]; // Only creates a new entity if the name isn't already registered
```

##### Access entities:

```cpp
auto ent = world.get(entityId);
auto ent = world.get("name");

// Created automatically
auto ent = world[entityId];
auto ent = world["name"];

// Check if an entity is valid
if (ent)
    // Do something with the entity
```

##### Register entity names:

```cpp
ent.bindName("name");
```

##### Copy entities:

```cpp
auto ent2 = ent; // ent2 is just another reference to the same entity!
auto ent3 = ent.clone(); // Copies all components and makes a new entity
```

##### Delete entities:

```cpp
ent.destroy();
world.destroy(ent.getId());
```

### Components

##### Define components:

TODO: Determine syntax

##### Bind/register component names to types:
Note: Name binding is global, and applies to all World instances. (Won't need this if the names are defined in components.)

```cpp
es::bindName<Component>("Component");
es_bindName(Component);
```

##### Accessing components (will create automatically if needed):

```cpp
auto ent = world[id];
auto& baseComp = ent["Component"];
auto& baseComp = ent.at("Component");
auto& comp = ent.at<Component>();
```

##### Accessing components (won't create):

```cpp
auto baseComp = ent.get("Component");
auto comp = ent.get<Component>();

Component comp;
ent >> comp;
```

##### Update/create components:

```cpp
ent << Component(params); // These can be chained like cout
ent.assign<Component>(params); // The efficient argument forwarding way
```

##### Deserialize (all are equivalent):

```cpp
ent << "Position 100 500";
ent["Position"] = "100 500";
ent.at<Position>() = "100 500";
```

##### Serialize:
Note: These utilize the implicit string cast, there is also a toString()

```cpp
std::string posStr;
posStr = ent.at<Position>();
posStr = ent["Position"];
```

##### Serialize (may remove these):

```cpp
std::string posStr;
ent.at<Position>() >> posStr;
ent["Position"] >> posStr;
```

##### Serialize all components:

```cpp
auto comps = ent.serialize(); // Returns a vector of strings
```

### Systems

TODO: Explain es::System and es::SystemContainer

### Events

TODO: Explain es::Events


## Author

Eric Hebert
