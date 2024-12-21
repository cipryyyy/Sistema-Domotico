cmake_minimum_required(VERSION 3.10)

project("Sistema di controllo domotico" VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)     # Da levare

include_directories(include)
include_directories(exceptions)

add_executable(SCD
    src/main.cpp
    src/CicloPreImpostato.cpp
    src/Device.cpp
    src/Interface.cpp
    src/ManualDevice.cpp
)
