target_sources(${PROJECT_NAME} PRIVATE

	"${CMAKE_CURRENT_SOURCE_DIR}/Emulator.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Emulator.cpp"

)

add_subdirectory("ALU")
add_subdirectory("CPU")
add_subdirectory("Instruction")
add_subdirectory("Memory")
add_subdirectory("Peripheral")
add_subdirectory("Register")
add_subdirectory("Spec")
