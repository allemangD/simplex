cmake_minimum_required(VERSION 3.10)
project(simplex)

set(CMAKE_CXX_STANDARD 17)

add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

option(GLM_TEST_ENABLE OFF)
add_subdirectory(vendor/glm)

add_subdirectory(vendor/versor)
get_property(VSR_INCLUDE_DIR GLOBAL PROPERTY VSR_INCLUDE_DIR)
target_include_directories(vsr PUBLIC ${VSR_INCLUDE_DIR})

add_subdirectory(framework)
add_subdirectory(simplex)
