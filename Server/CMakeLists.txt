cmake_minimum_required(VERSION 3.23)
project(Server)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib )

add_executable(Server
    ../Core/Core.h
    Server.h
    Server.cpp
    main.cpp
)
