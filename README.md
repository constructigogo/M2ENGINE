# Installation 

--- 
To install, simply clone the repo to your library folder in your project, or add it as a git submodule like the following :
```bash
cd [your library folder]
git submodule add https://github.com/constructigogo/M2ENGINE.git
git submodule update --init --recursive -f
```

M2Engine is made to be used as a CMake subdirectory.
to add it, simply put the following inside your own CMakeList.txt
```cmake
add_subdirectory(libraries/M2Engine/)
```


