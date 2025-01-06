cmake_minimum_required(VERSION 3.10)

project("Sistema di controllo domotico" VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo di build: Release o Debug" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

include_directories(include)
include_directories(exceptions)

add_executable(SmartHome
    src/main.cpp
    src/CPDevice.cpp
    src/Device.cpp
    src/Interface.cpp
    src/ManualDevice.cpp
)