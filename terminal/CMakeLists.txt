
cmake_minimum_required (VERSION 3.8)

project(x69Terminal VERSION 0.0.1)

include(CTest)



set(src_files 
	"source/${PROJECT_NAME}.cpp"
	"source/Colors.h"
	"source/RealTime.h"
)

add_library(${PROJECT_NAME} STATIC "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp" "${src_files}")

target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")

target_link_libraries(${PROJECT_NAME} PRIVATE tvision)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)

target_compile_definitions(${PROJECT_NAME} PRIVATE SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}")

add_subdirectory("source/emu_status")
add_subdirectory("source/file_editor")
add_subdirectory("source/file_view")
add_subdirectory("source/terminal_view")
add_subdirectory("source/register_view")
add_subdirectory("source/shell")

add_subdirectory("lib/tvision")
