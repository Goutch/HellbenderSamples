cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(Physics CXX)

add_subdirectory(dependencies/box3d)

add_executable(${PROJECT_NAME}
        main.cpp
        PhysicsSystem.cpp
        PhysicsSystem.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC HBE PUBLIC box3d)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/box3d"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        )

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${HBE_OUTPUT_DIR})