
add_library(x69Peripheral_Persistant SHARED "source/pf_persistant.cpp" "include/pf_persistant.h")
target_include_directories(x69Peripheral_Persistant PUBLIC "include" PRIVATE "source")
set_target_properties(x69Peripheral_Persistant PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)

