Sample run:
```
    cmake -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -G Ninja -B build
```
```
    cmake -B build -G Ninja -DCMAKE_C_COMPILER="C:/Development/Tools/msys64/clang64/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/Development/Tools/msys64/clang64/bin/clang++.exe"

Dependencies:
- CMAKE
- Raylib