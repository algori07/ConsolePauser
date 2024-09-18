# add support to windows xp
cmake_minimum_required(VERSION 3.13.5)

set(CMAKE_C_STANDARD 99)

project(ConsolePauser C)

option(CP_STATIC "Build static executable.(Currently support GNU and MSVC compiler)" OFF)

set(TARGET_NAME consolepauser)

if(WIN32)
	set(TARGET_NAME ConsolePauser)
endif()

add_executable(${TARGET_NAME} main.c)

if(CP_STATIC)
	if(MSVC)
		target_link_options(${TARGET_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
	elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
		target_link_options(${TARGET_NAME} PRIVATE "-static")
		target_link_options(${TARGET_NAME} PRIVATE "-static-libgcc")
	endif()
endif()

if(WIN32)
	install(TARGETS ${TARGET_NAME})
else()
	include(GNUInstallDirs)
	install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_BINDIE})
endif()