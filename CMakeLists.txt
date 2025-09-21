cmake_minimum_required (VERSION 3.8)
project ("LoopieEngine" VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Subdirectories
add_subdirectory("Loopie")
add_subdirectory("LoopieEditor")
#add_subdirectory("LoopieSandbox")

# Make LoopieEditor the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoopieEditor)