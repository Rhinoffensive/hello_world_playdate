#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#include "src/chip8/include/config.h"
#include "src/chip8/include/chip8.h"


static int update(void *userdata);


const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

struct chip8 chip8;

const char keyboard_map[CHIP8_TOTAL_KEYS] = {
        0, 0, 0, 0, 0, 0,
        0, 0, kButtonA, 0, 0, 0,
        0, 0, 0, 0};


int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
    (void) arg; // arg is currently only used for event = kEventKeyPressed

    if (event == kEventInit) {
        const char *err;
        font = pd->graphics->loadFont(fontpath, &err);

        if (font == NULL)
            pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

        SDFile *file = pd->file->open("Airplane.ch8", kFileRead);
        if (file == NULL) {
            pd->system->error("Couldn’t open file % s", "Airplane.ch8");
            pd->system->logToConsole(pd->file->geterr());

        } else {
            pd->system->logToConsole("File loaded successfully");

            pd->file->seek(file, 0, SEEK_END);
            int size = pd->file->tell(file);
            pd->system->logToConsole("File size : %d", size);
            pd->file->seek(file, 0, SEEK_SET);

            char buff[size];

            int res = pd->file->read(file, buff, size);
            if (res != 1) {
                printf("Failed to read from file");
//                return -1;
            }
//          struct chip8 chip8;
            chip8_init(&chip8);
            pd->system->logToConsole("init run!");
            chip8_load(&chip8, buff, size);
            chip8_keyboard_set_map(&chip8.keyboard, keyboard_map);

        }



        // Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
        pd->system->setUpdateCallback(update, pd);
    }

    return 0;
}


static int update(void *userdata) {


    PlaydateAPI *pd = userdata;

    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(font);



    PDButtons current;

    pd->system->getButtonState(&current, NULL, NULL);

//    pd->system->logToConsole("Current press %d", current);
    int vkey = chip8_keyboard_map(&chip8.keyboard, current);
//    pd->system->logToConsole("Key press %d", vkey);
    if (vkey != -1) {
        chip8_keyboard_down(&chip8.keyboard, vkey);
    }


    int displayHeight = pd->display->getHeight();
    int displayWidth = pd->display->getWidth();

    int heightMul = displayHeight / CHIP8_HEIGHT;
    int widthMul = displayWidth / CHIP8_WIDTH;

    for (int cx = 0; cx < CHIP8_WIDTH; cx++) {
        for (int cy = 0; cy < CHIP8_HEIGHT; cy++) {
            if (chip8_screen_is_set(&chip8.screen, cx, cy)) {
                pd->graphics->fillRect(cx * widthMul, cy * heightMul, widthMul, heightMul,
                                       kColorBlack);
            } else {
                pd->graphics->fillRect(cx * widthMul, cy * heightMul, widthMul, heightMul,
                                       kColorWhite);
            }
        }
    }

    if (chip8.registers.delay_timer > 0) {

        chip8.registers.delay_timer -= 1;

    } else {
//        float acc = 0.0f;
//        while(acc < 5000.0f){
//            acc += pd->system->getElapsedTime();
//        }

        pd->system->resetElapsedTime();

//        msleep(3);

    }

    if (chip8.registers.sound_timer > 0) {
        //fprintf(stdout, "\aBeep!\n" );
//            Beep(15000, 10 * chip8.registers.sound_timer);
        chip8.registers.sound_timer = 0;
    }

    unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.PC);
    chip8.registers.PC += 2;
    chip8_exec(&chip8, opcode);

//    PDButtons current;
    pd->system->getButtonState(&current, NULL, NULL);

    chip8_keyboard_all_up(&chip8.keyboard);


//    dx = dy = 0;
////    pd->system->logToConsole("log");
//    if (current & kButtonUp) {
//        dy = -1;
//    } else if (current & kButtonDown) {
//        dy = 1;
//    }
//    if (current & kButtonLeft) {
//        dx = -1;
//    } else if (current & kButtonRight) {
//        dx = 1;
//    }

//    if (x < 0 || x > LCD_COLUMNS - TEXT_WIDTH)
//        dx = -dx;
//
//    if (y < 0 || y > LCD_ROWS - TEXT_HEIGHT)
//        dy = -dy;

//    x += dx;
//    y += dy;
    pd->system->drawFPS(0, 0);


    return 1;
}



