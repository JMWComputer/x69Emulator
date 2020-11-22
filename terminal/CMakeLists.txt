
cmake_minimum_required (VERSION 3.8)

add_library(x69Terminal STATIC "include/x69Terminal.h" "source/x69Terminal.cpp")
target_include_directories(x69Terminal PUBLIC "include" PRIVATE "source")

add_subdirectory("lib/tvision")
target_link_libraries(x69Terminal PRIVATE tvision)
