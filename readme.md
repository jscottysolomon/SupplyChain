Sample run:
```
  cmake -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -G Ninja -B build
```
```
  cmake -B build -G Ninja -DCMAKE_C_COMPILER="C:/Development/Tools/msys64/clang64/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/Development/Tools/msys64/clang64/bin/clang++.exe"
```

Dependencies:
- CMAKE
- Raylib

Todo:
- Unload contents of truck to factory
- Load contents of factory to truck
- reservatin of resoures of factory to trucks

Truck Unload
- Unload x of widget A, y of widget B
- Unload until truck inventory is empty

Truck Reload
Rules
  1. Load exactly x of Widget A
  1. Load atleast x of Widget A
  1. Load reserved widgets
  2. Load up to x of Widget A
  2. Load as much Widget A as possible, then widget B, then... (list of widgets to load in priority)
  3. Load anything in whitelist
  3. Load anything not in blacklist
Finish
  Load until full
  Load until x space left
  x time has passed
  loading a widget takes longer than x time

Priority queue of Widgets to Load