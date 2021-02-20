cmake_minimum_required (VERSION 3.8)

project(x69Emulator)

include(CTest)

set(emu_sources 
	"source/Emulator.h"
	"source/Emulator.cpp"
)

add_library(${PROJECT_NAME}Lib STATIC "include/${PROJECT_NAME}Lib.h" "source/${PROJECT_NAME}Lib.cpp" "${emu_sources}")
target_include_directories(${PROJECT_NAME}Lib PUBLIC "include" PRIVATE "source")
set_target_properties(${PROJECT_NAME}Lib PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} "executable/${PROJECT_NAME}.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DSOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}/" -DENVIRONMENT_ROOT="./")

add_subdirectory("peripherals")

install(TARGETS ${PROJECT_NAME}Lib DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(FILES "include/${PROJECT_NAME}Lib.h" DESTINATION "${CMAKE_INSTALL_PREFIX}include")

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
