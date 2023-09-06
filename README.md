# Installation 

--- 
M2Engine can be used as a library or as an app (the latter is not recommanded but possible)

To install it as a library, simply clone the repo to your library folder in your project, or add it as a git submodule like the following :
```bash
cd [your library folder]
git submodule add https://github.com/constructigogo/M2ENGINE.git
git submodule update --init --recursive -f
```

M2Engine is made to be used as a CMake subdirectory.
to add it, simply put the following inside your own CMakeList.txt
```cmake
add_subdirectory([your library folder]/M2Engine/)
```

# Usage

---
M2Engine will look for the `shaders` folder at your project root

To create an App, use the following include, and create a child class inheriting from `App`
```cpp
#include "[your library folder]/M2ENGINE/Engine/App.h"
```



