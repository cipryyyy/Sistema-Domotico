cmake_minimum_required(VERSION 3.10)

project("Sistema di controllo domotico" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_BUILD_TYPE Release)

include_directories(include extra)

add_executable(SmartHome
    src/main.cpp
    src/CPDevice.cpp
    src/Device.cpp
    src/Interface.cpp
    src/ManualDevice.cpp
)
