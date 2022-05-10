cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)


set(PLAYDATE_SDK_PATH /home/rtt/Documents/PlaydateSDK-1.9.3)
set(ENVSDK ${PLAYDATE_SDK_PATH})
set(SDK ${ENVSDK})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})

	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()



set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME hello_world)
set(PLAYDATE_GAME_DEVICE hello_world_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

add_subdirectory(src/chip8)

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} ${SDK}/C_API/buildsupport/setup.c main.c src/chip8/src/chip8.c src/chip8/src/chip8keyboard.c src/chip8/src/chip8stack.c src/chip8/src/chip8screen.c src/chip8/src/chip8memory.c)
#	target_link_libraries(${PLAYDATE_GAME_DEVICE} PUBLIC chip8lib)
	message("Link complete")
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED main.c src/chip8/src/chip8.c src/chip8/include/chip8.h src/chip8/src/chip8keyboard.c src/chip8/include/chip8keyboard.h src/chip8/src/chip8stack.c src/chip8/include/chip8stack.h src/chip8/src/chip8screen.c src/chip8/include/chip8screen.h src/chip8/src/chip8memory.c src/chip8/include/chip8memory.h)
	message("Else")
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

