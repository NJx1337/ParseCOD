# ParseCOD
A program to parse .cod files from the game Anno 1602 on runtime speed.
This is a very early code. A lot will change!

## What it can do
- Parse constants and HAUS object (to CODTileInfo) in haeuser.cod, can be a base to process similar .cod files as well
- Save readable .txt file from .cod, that can be edited and loaded aswell
- Print all tile infos from loaded CODTileInfo
- Predefine constants from the game.exe in CODIntConst.h -> Existing one were re'd from the EN GOG game version
- Save/Load a cached version of the loaded TileInfos from haeuser.cod, for easier dev, also can be edited and loaded

## How to run
1. The project was made in VS 2022. It is the safest way to compile it there sucessfully.
2. In the main.cpp define the path to the .cod file you want to load (currently only haeuser.cod supported)
3. On successfully parsing of the haeuser.cod the TileMapping object will be filled with infos to the tile types, that can be used to render the island tiles of a savegame.
4. Enjoy some minor memory leaks, haven't even looked into them yet.

## ToDo
- Faster and more efficient parsing
- Even mooore C
- More tile information
- Multiversion support (+ Multilanguage)
- Multiplatform support
- Merge this project into an even bigger one (that can do more, from doing some simple representations of game stuff, to even rendering, have I mentioned: I love game design! ;-) )

## FAQ
### 1. Why another tool for the over 27 year old game?
Because I love that game. And I love programming and reverse engineering stuff, as well as game design.

### 2. A similar tool xyz exists already, why not continuing on that one?
I know there might be already a few other amazing tools that can do the same stuff out there, maybe even better, but my goal is to have fun in making it my way, my style and mostly in pure C without a lot of dependencies.

### 3. Is there Linux, Mac support?
Currently I am focussing on the core functionality and not on compatiblity. Since it the tool has no or not much dependencies, it could be easy to migrate it to other systems.

### 4. Why you dont use CMake, etc?
Currently I am focussing on the core functionality and not on compatiblity.

### 5. I want to use the entire code or parts in my own software. Are there any requirements?
Feel free to use my code, expand it, make it better, learn from it, heck you can even use it in your comercial projects if you want, it's open source. Just follow the LICENSE. ;-)

### 6. Which game version are you using primarly for developement?
I am using the Anno 1602 Version from GoG in English, planning to add all other languages (from GoG) too in future and the History Edition.

### 7. Whats the goal with this project?
Make this project run on Windows 95 and VS 6.0. Just joking: ;-)

My goal is to have fun for myself and also make the available to others, so people can learn from it. And a serious goal is to make it as perfect as possible in reading game files: Read everything from the game files, to have no need to manually define stuff.

If I find more free time for this project, there might come more in future. :-)
