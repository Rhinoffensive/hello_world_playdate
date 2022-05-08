//
//  main.c
//  Extension
//
//  Created by Dave Hayden on 7/30/14.
//  Copyright (c) 2014 Panic, Inc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "include/config.h"

static int update(void *userdata);


const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

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


            int res = pd->file->read(file,buff,size);
            if (res != 1) {
                printf("Failed to read from file");
//                return -1;
            }




        }
        // Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
        pd->system->setUpdateCallback(update, pd);
    }

    return 0;
}


#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400 - TEXT_WIDTH) / 2;
int y = (240 - TEXT_HEIGHT) / 2;
int dx = 1;
int dy = 1;


static int update(void *userdata) {
    PlaydateAPI *pd = userdata;

    pd->graphics->clear(kColorWhite);
    pd->graphics->setFont(font);


    void *text = "PlayDate";

//    pd->graphics->drawText(text, strlen(text), kASCIIEncoding, x, y);
//    pd->graphics->drawText((void *)dx,strlen((void *)dx),kASCIIEncoding,5,5);
//    pd->graphics->drawText((void *)(char*)dy,strlen((void *)(char*)dy),kASCIIEncoding,5,10);
//	pd->graphics->drawText("Hello World!", strlen("Hello World!"), kASCIIEncoding, x, y);
//    pd->graphics->setDrawMode(kDrawModeFillBlack);
    pd->graphics->fillRect(0,0,100,100,kColorBlack);

    int displayHeight = pd->display->getHeight();
    int displayWidth = pd->display->getWidth();

    int heightMul = displayHeight/ CHIP8_HEIGHT;
    int widthMul = displayWidth/ CHIP8_WIDTH;

    for (int cx = 0; cx < CHIP8_WIDTH; cx++) {
        for (int cy = 0; cy < CHIP8_HEIGHT; cy++) {
            pd->graphics->fillRect(cx*widthMul,cy*heightMul,widthMul,heightMul,(cx+cy)%2==1?kColorBlack:kColorWhite);

//            if (chip8_screen_is_set(&chip8.screen, x, y)) {
//                SDL_Rect r;
//                r.x = x * CHIP8_WINDOW_MULTIPLIER;
//                r.y = y * CHIP8_WINDOW_MULTIPLIER;
//                r.w = CHIP8_WINDOW_MULTIPLIER;
//                r.h = CHIP8_WINDOW_MULTIPLIER;
//                SDL_RenderFillRect(renderer, &r);
//            }
        }
    }

    PDButtons current;
    pd->system->getButtonState(&current, NULL, NULL);

    dx = dy = 0;
//    pd->system->logToConsole("log");
    if (current & kButtonUp) {
        dy = -1;
    } else if (current & kButtonDown) {
        dy = 1;
    }
    if (current & kButtonLeft) {
        dx = -1;
    } else if (current & kButtonRight) {
        dx = 1;
    }

//    if (x < 0 || x > LCD_COLUMNS - TEXT_WIDTH)
//        dx = -dx;
//
//    if (y < 0 || y > LCD_ROWS - TEXT_HEIGHT)
//        dy = -dy;

    x += dx;
    y += dy;
    pd->system->drawFPS(0, 0);


    return 1;
}



