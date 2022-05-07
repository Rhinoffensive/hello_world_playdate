add_library(chip8lib chip8.c chip8keyboard.c chip8memory.c chip8screen.c chip8stack.c)
target_include_directories(chip8lib PUBLIC include)