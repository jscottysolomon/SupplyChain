cmake_minimum_required(VERSION 3.15)

project(CHAIN LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

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

target_include_directories(CHAIN PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/external"
  "${CMAKE_SOURCE_DIR}/external/imgui"
)
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  target_link_libraries(CHAIN PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/WEB/libraylib.a"
  )

  target_link_options(CHAIN PRIVATE
    -sUSE_GLFW=3
    -sASYNCIFY
    -sALLOW_MEMORY_GROWTH=1
  )
elseif (WIN32)
  target_link_libraries(CHAIN PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/MSVC/raylib.lib"
    opengl32
    gdi32
    winmm
    user32
    shell32
  )
  target_link_options(CHAIN PRIVATE
    -sUSE_GLFW=3
    -sASYNCIFY
    -sALLOW_MEMORY_GROWTH=1
    -sGLFW_SCALE_TO_MONITOR=0
  )
elseif (APPLE)
  target_link_libraries(CHAIN PRIVATE
    raylib
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
  )
elseif (UNIX AND NOT APPLE)
  target_link_libraries(CHAIN PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/Linux/libraylib.a"
    GL
    m
    pthread
    dl
    rt
    X11
  )
endif()

if (MSVC)
  target_compile_options(CHAIN PRIVATE
    /W4
    /Od
    /Zi
    /permissive-
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-missing-field-initializers
  )

  target_link_options(CHAIN PRIVATE
    /DEBUG
  )

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(CHAIN PRIVATE
    -g
    -O0
    -Wall
    -Wno-missing-braces
    -pedantic
    -Wno-unused-variable
    -Wsign-compare
    -Wno-delete-non-virtual-dtor
    -fdiagnostics-color=always
  )

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(CHAIN PRIVATE
      -fpermissive
    )
  endif()
endif()