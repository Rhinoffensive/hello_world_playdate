add_library(chip8lib src/chip8.c src/chip8keyboard.c src/chip8memory.c src/chip8screen.c src/chip8stack.c)
target_include_directories(chip8lib PUBLIC include)
