cmake_minimum_required(VERSION 3.10)

project(SistemaDomotico)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/CicloPreImpostato.cpp
    src/Device.cpp
    src/Interface.cpp
    src/ManualDevice.cpp
    src/main.cpp
)

add_executable(main ${SOURCES})

#Spudoratamente copiato da chatgpt e si vede <3