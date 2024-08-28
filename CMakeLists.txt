# add support to windows xp
cmake_minimum_required(VERSION 3.13.5)


project(ConsolePauser C)

if(WIN32)
add_executable(ConsolePauser main.c)
else()
add_executable(consolepauser main.c)
endif()