cmake_minimum_required(VERSION 3.10)

project("Sistema di controllo domotico" VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Imposta il tipo di build su Release per abilitare ottimizzazioni
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo di build: Release o Debug" FORCE)
endif()

# Aggiunge l'opzione -O2 per il tipo di build Release
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

include_directories(include)
include_directories(exceptions)

add_executable(SCD
    src/main.cpp
    src/CPDevice.cpp
    src/Device.cpp
    src/Interface.cpp
    src/ManualDevice.cpp
)

#Sì, lo ho fatto con chatGPT