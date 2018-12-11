cmake_minimum_required(VERSION 3.13)
project(gl_template)

set(CMAKE_CXX_STANDARD 17)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

option(GLM_TEST_ENABLE OFF)
add_subdirectory(vendor/glm)

add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_subdirectory(framework)
add_subdirectory(glapp)
