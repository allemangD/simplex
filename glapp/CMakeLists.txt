project(sample)

add_executable(sample
        src/main.cpp)

target_link_libraries(sample
        glad
        glm
        framework)
