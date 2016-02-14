ConfigFile
==========

This C++14 library reads simple configuration files, which can be used in all kinds of software. You can even modify and save configuration files, or you can simply use it to read user settings.

The main purpose of this project is to have a simple file format which can be used extremely easily in code, and to reduce boilerplate/parsing code. Users of software using this format can easily modify these files without worrying about a lot of syntax. This is a very loose format, which ignores whitespace, and has dynamic data-types.

The file format looks like this:

```dosini
someOption = someValue
```

The left side is the name of the option, and the right is its value. The equals symbol represents that the option is being set to that value.

Example file
------------

```dosini
[ExampleSection]

# Numeric types:
someNumber = 500
eulersNumber = 2.718281828459045

# Booleans:
someBoolean = true
anotherBool = FALSE
# The values of booleans are not case sensitive.

# Booleans can also be in numeric form.
# Zero is false, any non-zero number is true.
bool1 = 0
bool2 = 100

# Strings:
name = "Qwerty"
color = "Blue"

# Option names are case sensitive:
number = 42
NUMBER = 250
# These are two different values.
NUMBER = 99999
# Options of the same name will rewrite the values of previous options.
# number is 42
# NUMBER is 99999

# Arrays:
myArray = {
    "Some text.",
    12345,
    {
        27.52,
        "Inside another array",
        true
    },
    "Back to outer array"
}
# Please refer to the "Arrays" section for more information.

[Section2]

# Each section has a different "scope"
someNumber = 1000
```

Features (file format)
----------------------

* Options can be named almost anything, except starting with a comment symbol or section symbol.
* The options can be any of these types:
  * Integers (-1, 0, 1, 2)
  * Decimals (0.0123, 99.765, -346.2)
  * Booleans (true, false, TRUE, FALSE, 0, 1)
  * Strings ("Insert text here")
    * See Strings description for more details.
  * Arrays ({element1, element2, element3})
    * See Arrays description for more details.
* Comments and sections are also supported.
* Extra whitespace is ignored around mostly everything.
  * New lines still have meaning though.

Features (classes)
------------------

* Can load/save from/to either a file or string
  * Can automatically save the file on object destruction
* Contains an in-memory std::map of the options
* Can use operator() to easily access/create/modify options
* Supports using a std::map to load default options from
  * This means the user isn't required to make a configuration file, and can just add the options they wish to change.
* You can check if an option exists before trying to access it (which would just create a new one)
* Has begin() and end() iterators to allow iterating through the sections and options with a range based for loop
* Can erase options and sections
* Option class:
  * Supports using a default value and valid range
    * If the option is being set to a value out of range, it won't be set
  * Supports setting/getting as all of the types listed in file format features
  * All type conversion is done on set, so that accessing is always fast

Strings
-------

NOTE: This is changing in the near future, all strings must have quotes and use proper escape codes for special characters such as newlines, whitespace, etc.

How strings are handled:

```dosini
someString =    this is a test
```

This string is interpreted as "this is a test". It ignores all whitespace "around" the data.
It is recommended to use quotes for strings, so that it is interpreted as you expect:

```dosini
someString = "    this is a test"
```

Symbols in strings work fine too, including quotes:

```dosini
str = "!@#$%^&*()"""""""_+-="
```

The first and last quote are used for determining what is contained in the string, so there is no need to escape characters.

Arrays
------

Arrays are fully dynamic and jagged, meaning you can have arrays within arrays with more arrays (and so on). Each element of the array can be an Option, or another array of Option objects. In memory they are stored as a type of tree structure. They are stored inside of the Option class just like all of the other types.

#### Example file with array:

```dosini
colors = {
    "Red",
    "Blue",
    "Green",
    "Yellow"
}
```

See example array uses below for more information.

Sections
--------

* "Sections" can help to organize or separate options
* If you use sections in your code, then they must be specified correctly, in order for options
    to be distinguished from options in other sections.
* If no sections are used, a default section is used, which is just an empty string.
  * This also applies to options in the beginning of the file before any section headers.
  * Specifying an empty string section with "[]" would have the same behavior as using no section.
  * You can even use "" as a section in your code which refers to the default section.
    * Note that this differs from the current section.

#### Example file with sections:

```dosini
[SomeSection]
option = value
anotherOption = 123

[AnotherSection]
option = 5000
```

So "option" will be read as "value" in SomeSection, but 5000 in AnotherSection.
Also, "anotherOption" only exists in SomeSection, so you will not get its value with AnotherSection.

Comments
--------

* Comments can be made with most symbols (//, #, ::, and ;)
* Comments MUST be on their own lines
  * The symbols are checked only in the beginning of each line
  * The whole line is ignored if it is a comment
* Multi-line comments are also supported with /* and */
* Currently, writing configuration files removes all comments. This will be changed in the future.

Example usage of classes
------------------------

You can create cfg::File objects, which can load/save configuration files. Loading one will actually keep an in-memory std::map of all of the options, so accessing/changing/creating options in your program will be fast. Then, if you want to, you can write the changes back into the same file or into a new file.

### Loading/saving configuration files

#### Loading files

```cpp
// First, you will need to include the header file:
#include "configfile.h"

// Load a configuration file:
cfg::File config("sample.cfg");
if (!config)
{
    // Error loading
}

// Or you can load it after you construct the object:
cfg::File config;
if (!config.loadFromFile("sample.cfg"))
{
    // Error loading
}
```

#### Saving files

Save to a file:

```cpp
cfg::File config;
if (!config.writeToFile("saved.cfg"))
{
    // Error saving
}
```

You can also save the last loaded file:

```cpp
cfg::File config("sample.cfg");
if (!config.writeToFile())
{
    // Error saving
}
```

You can also enable the Autosave flag, as shown in "Loading with flags".

#### Loading with default options

You can specify default options in code:

```cpp
const cfg::File::ConfigMap defaultOptions = {
{"ExampleSection", {
    {"someOption", cfg::makeOption(false)},
    {"pi", cfg::makeOption(3.14159265358979)},
    {"name", cfg::makeOption("Test")},
    {"percent", cfg::makeOption(0, 0, 100)}
}}};
```

Load a file using those default options:

```cpp
cfg::File config("sample.cfg", defaultOptions);
```

If any of those options do not exist in the file, the defaults will be used.

#### Loading with flags

Currently, there are three flags:

* Warnings (Print messages when options are out of range)
* Errors (Print errors when loading/saving files)
* Autosave (Automatically save the last file loaded on destruction)

By default, all of these are disabled. You can enable these flags like so:

```cpp
cfg::File config("sample.cfg", cfg::File::Autosave);
// Now when config goes out of scope, it will automatically be saved.

// Same as above:
cfg::File config("sample.cfg");
config.setFlags(cfg::File::Autosave);
// The last parameter in the constructor is passed to setFlags().

// You can enable multiple flags like this:
cfg::File config("sample.cfg", cfg::File::Autosave | cfg::File::Errors);

// Or just enable all of them:
cfg::File config("sample.cfg", cfg::File::AllFlags);

// You can enable/disable specific flags later on:
config.setFlag(cfg::File::Errors, true); // Enable
config.setFlag(cfg::File::Errors, false); // Disable
config.setFlag(cfg::File::Errors); // Easier way to enable
```

Note that "setFlags" will reset all of the flags to what is specified, while "setFlag" will only modify the flag that is specified.

### Manipulating options

#### Option ranges

Notice in the previous example, there were three parameters used in cfg::makeOption(). The parameters are as follows:

* Default value (required)
* Minimum value (optional)
* Maximum value (optional)

Whenever a value in the configuration file is loaded or an option is being set in your program, it will only be set if the value is within that range. Assuming the above code is included, here is an example:

```cpp
config.useSection("ExampleSection"); // Use the correct section
config("percent") = 50; // OK, value is in range
config("percent") = 9000; // Not set, because value is out of range
// "percent" ends up as 50
```

Note: Currently, only numeric ranges are supported. Anything more advanced should be done in code where the options are read in.

#### Reading options

```cpp
// Read an option as an int:
int someNumber = config("someNumber").toInt();

// Notice the .toInt() above. This is because using operator() returns
// a reference to a cfg::Option object, which can be read as different types.

// Some other types that it can be read as:
auto str = config("someString").toString();
double dec = config("someDouble").toDouble();
bool someBool = config("someBool").toBool();

// You can also get the option as almost any value that can cast from a double:
auto someValue = config("someOption").to<unsigned short>();

// There is an implicit cast operator for converting to a string:
std::string str = config("someString");
```

#### Modifying options

Options can be set to values of different types:

```cpp
config("someNumber") = 200;
config("someNumber") = 3.14159;
config("someNumber") = "Some string";
config("someNumber") = true;
```

#### Accessing options with sections

```cpp
// The second parameter of operator() is the section to use:
config("test", "NewSection") = 5;

// You can alternatively set the current section to use by default:
config.useSection("NewSection");

// So that this section will be used when nothing is specified:
config("test") = 5;

// Both will set "test" in "NewSection" to 5.
```

#### Iterating through cfg::File

If you need to access options/sections in a config file, without knowing the names, you can do so by iterating through it.

To iterate through the sections:

```cpp
cfg::File config("sample.cfg");
for (auto& section: config)
{
    cout << "Section name: " << section.first << endl;
    // "section.second" contains the contents of the section
    // The type of section.second is cfg::File::Section, or std::map<std::string, cfg::Option>
}
```

To iterate through all of the options in every section:

```cpp
cfg::File config("sample.cfg");
for (auto& section: config)
{
    for (auto& option: section)
    {
        // option.first contains the name of the option
        cout << "Option name: " << option.first << endl;

        // option.second contains the cfg::Option object
        cout << "Option value: " << option.second << endl;
    }
}
```

To iterate through all of the options in a specific section:

```cpp
cfg::File config("sample.cfg");
for (auto& option: config.getSection("SpecificSection"))
    // Same as above example
```

### Using arrays

#### Reading values

sample.cfg

```dosini
colors = {
    "Red",
    "Blue",
    "Green",
    "Yellow"
}
```

Print the 3rd color (green):

```cpp
cfg::File config("sample.cfg");
std::cout << config("colors")[2] << std::endl;
// Note: Arrays are 0-based.
// Note: The << operator is overloaded and just calls toString().
```

You can iterate through the array like so:

```cpp
cfg::File config("sample.cfg");
// Print all of the elements
for (auto& col: config("colors"))
    std::cout << col << std::endl;
```

This will only get you the outer-most elements in the array. If you need to access things deeper, you would just access the element's array the same way, since all Options contain an array of more Options (but they start out empty).

Here is an example of a jagged array:

sample2.cfg

```cpp
stuff = {
    {
        "values",
        123,
        456
    },
    {
        "more values",
        1.2345,
        99,
        3000.987
    }
}
```

You can iterate through the jagged array like so:

```cpp
cfg::File config("sample2.cfg");
for (auto& arr: config("stuff"))
    for (auto& elem: arr)
        std::cout << elem << std::endl;
```

#### Modifying values

To add/remove options from arrays, you can use the push and pop methods in the Option class.

```cpp
// This can also be an option from a cfg::File object
Option test;

// push() takes an option object
test.push(cfg::makeOption("Some text"));

// A blank Option will be added if nothing is passed in
test.push();

// push() also returns a reference to the newly added option object
test.push() = "Some more text";
test.push().push().push();

// Using operator << is an alternative to push()
test << "Some text" << 123 << "More text" << true << 3.14;

// Remove the last option with pop()
test.pop();

// Remove all of the options, reverting the option back to a single element
test.clear();
```

To change existing values, you can use operator[] just like you would to read the values:

```cpp
// Add some values to "test"
Option test;
test.push() = "Testing";
test.push() = 500;
test.push() = 99.999;

// Change the values in "test"
test[1] = "Five hundred";

// Change/access the last value in the array
test.back() = 45.67;

// You can start new arrays within already existing elements:
test[2].push() = "I'm in an array inside of another array"
```

For more ways of using the cfg::File and cfg::Option classes, please refer to the header files. There are comments that have information about what everything does. In the future I'll use a documentation generator so everything is properly documented.
