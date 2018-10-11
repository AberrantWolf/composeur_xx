cmake_minimum_required(VERSION 3.4)
project("ComposeurXX")

add_subdirectory(thirdparty)

add_executable(ComposeurXX src/main.cpp)

target_link_libraries(ComposeurXX portaudio_static)