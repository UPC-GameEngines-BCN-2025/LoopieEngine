cmake_minimum_required (VERSION 3.8)
project ("LoopieEngineCMake" VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Subdirectories
add_subdirectory("Loopie")
add_subdirectory("LoopieEditor")
#add_subdirectory("LoopieSandbox")


#Filters
set_target_properties(Loopie PROPERTIES FOLDER "Core")
set_target_properties(LoopieEditor PROPERTIES FOLDER "Editor")
#set_target_properties(LoopieSandbox PROPERTIES FOLDER "Misc")