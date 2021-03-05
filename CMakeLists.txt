cmake_minimum_required (VERSION 3.8)

project(x69Emulator)

include(CTest)

set(emu_sources 
	"source/Emulator.h"
	"source/Emulator.cpp"
)

add_library(${PROJECT_NAME} STATIC "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp" ${emu_sources})
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)

add_subdirectory("peripherals")

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(FILES "include/${PROJECT_NAME}.h" DESTINATION "${CMAKE_INSTALL_PREFIX}include")
