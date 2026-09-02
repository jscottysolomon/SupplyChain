cmake_minimum_required(VERSION 3.15)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(CHAIN LANGUAGES CXX)

set(SRC "src")

file(GLOB SOURCES CONFIGURE_DEPENDS 
"src/*.cpp"
"src/core/*.cpp"
"src/util/*.cpp"
"src/mediators/*.cpp"
"src/ui/*.cpp"
"external/imgui/*.cpp"
)

add_executable(CHAIN ${SOURCES})

target_compile_features(CHAIN PRIVATE cxx_std_20)
set_target_properties(CHAIN PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

#set(CXXFLAGS
#"-g -O0 -Wall -std=c++20 -Wno-missing-braces -pedantic -Wno-unused-variable -Wunused-result -Wno-delete-non-virtual-dtor -fpermissive -stdlib=libc++"
#)

target_include_directories(CHAIN
  PRIVATE 
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/external/
  ${CMAKE_SOURCE_DIR}/external/imgui/
  ${CMAKE_SOURCE_DIR}/lib/
)


if (MSVC)
  target_link_libraries(CHAIN PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/MSVC/raylib.lib"
    opengl32
    gdi32
    winmm
    user32
    shell32
  )

  target_compile_options(CHAIN PRIVATE
    /W4
    /Zi
    /fsanitize=address
  )

  target_link_options(CHAIN PRIVATE
    /fsanitize=address
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(CHAIN PRIVATE
    -g
    -O0
    -Wall
    -Wno-missing-braces
    -pedantic
    -Wno-unused-variable
    -Wsign-compare
    -Wno-delete-non-virtual-dtor
    -fpermissive
    -fcolor-diagnostics
  )

  target_link_libraries(CHAIN PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/libraylib.a"
    opengl32
    gdi32
    winmm
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(CHAIN PRIVATE
    -g
    -O0
    -Wall
    -Wno-missing-braces
    -pedantic
    -Wno-unused-variable
    -Wsign-compare
    -Wno-delete-non-virtual-dtor
    -fpermissive
    -std=c++20
  )
  target_link_libraries(CHAIN PRIVATE "${CMAKE_SOURCE_DIR}/lib/libraylib.a" opengl32 gdi32 winmm)
  add_compile_options (-fdiagnostics-color=always)
else()
  message(WARNING "Unsupported compiler. No specific libraries will be linked.")
endif ()
