project(framework)

add_library(framework
        src/hello.cpp)

target_link_libraries(framework
        glfw)

target_include_directories(framework
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        src)

export(TARGETS framework glfw FILE FrameworkConfig.cmake)