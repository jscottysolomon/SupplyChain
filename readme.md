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
- Create Warehouse & Wholeseller objects (factory wrapper)
- Select truck and create lines to show the route/stops they're gonna take on the map. 
- Sell to Warehouses/Wholesellers
- reservatin of resoures of factory to trucks
- Remove redundancies in routing (going on the same road twice)
- Make roads one lane each
- Add turn segment for trucks to enter/exit factory turn ins
- Add four way stops
- Add roundabouts
- Add traffic lights
- Ability to pause/unpause time/traffic or do a speed increase/decrease
- Raylib camera object

Bottlenecks (Increasable)
- Truck can only hold 20-40 amount of widgets
- Truck can only hold 1-2 type of widgets
- Factories can only produce 1-3 types of widgets
- Factories only have 1-2 docks (What should the truck do? Wait, skip, reroute?)
- Warehouses/Wholesellers will by X widget types, but only has Y docking stations
- Warehouse will by widget X for Y price, but widget A (made from widget X) can be sold for more

Potential Features
- Production costs X amount for Y widget


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