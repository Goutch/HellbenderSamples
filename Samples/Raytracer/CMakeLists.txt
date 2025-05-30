cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(Raytracer CXX)

#set(NRD_EMBEDS_DXBC_SHADERS OFF)
#set(NRD_EMBEDS_DXIL_SHADERS OFF)
#set(NRD_SHADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders/denoising/nrd)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/NRD)

add_executable(${PROJECT_NAME}
        main.cpp
        RaytracingScene.cpp
        RaytracingModelParser.cpp
        DataStructures.h
        Raytracer.cpp
        Raytracer.h
        Denoiser.cpp
        Denoiser.h)

target_link_libraries(${PROJECT_NAME} PUBLIC HBE)
target_include_directories(${PROJECT_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${HBE_OUTPUT_DIR}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${HBE_OUTPUT_DIR})