cmake_minimum_required (VERSION 3.8)

project(x69Emulator)

include(CTest)

add_library(${PROJECT_NAME} STATIC "include/${PROJECT_NAME}.h" "source/${PROJECT_NAME}.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)


add_subdirectory("source")


add_subdirectory("peripherals")



###
### TEMPORARY
###

add_executable(x69EMUTEST "test/test.cpp")
target_link_libraries(x69EMUTEST  PUBLIC ${PROJECT_NAME})
set_target_properties(x69EMUTEST PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
target_compile_definitions(x69EMUTEST PRIVATE SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
###
###
###


install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(FILES "include/${PROJECT_NAME}.h" DESTINATION "${CMAKE_INSTALL_PREFIX}include")
