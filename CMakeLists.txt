# add support to windows xp
cmake_minimum_required(VERSION 3.13.5)

set(CMAKE_C_STANDARD 99)

project(ConsolePauser C)

option(CP_STATIC "Build static executable.(Currently support GNU and MSVC compiler)" OFF)

set(CP_TARGET_NAME consolepauser)

if(WIN32)
	set(CP_TARGET_NAME ConsolePauser)
endif()

add_executable(${CP_TARGET_NAME} main.c)

if(CP_STATIC)
	if(MSVC)
		target_compile_options(${CP_TARGET_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
	elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
		target_link_options(${CP_TARGET_NAME} PRIVATE "-static")
		target_link_options(${CP_TARGET_NAME} PRIVATE "-static-libgcc")
	endif()
endif()

install(TARGETS ${CP_TARGET_NAME})
