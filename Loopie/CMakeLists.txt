cmake_minimum_required (VERSION 3.8)

project ("Loopie" VERSION 0.1.0 LANGUAGES C CXX)


set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE  SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

find_package(spdlog REQUIRED)
find_package(glad REQUIRED)
find_package(opengl REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)


add_library(${PROJECT_NAME} ${SOURCES})
add_library(loopie-core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${SRC_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC
	spdlog::spdlog
	SDL3::SDL3
	glad::glad
	OpenGL::GL
	glm::glm
)


target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE _CRT_SECURE_NO_WARNINGS )

