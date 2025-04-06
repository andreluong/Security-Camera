// Module for displaying text to LCD.
// After each boot, requires sudo chmod a+rw /dev/spidev0.* or sudo ./beatbox

#ifndef _LCD_DISPLAY_H_
#define _LCD_DISPLAY_H_

#define MAX_LENGTH 100
#define X_POS 5
#define Y_POS 20
#define Y_OFFSET 20
#define MAX_INFO_LINES 4
#define BACKLIGHT 1023

#include "DEV_Config.h"
#include "LCD_1in54.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>

#include <thread>

class LCDDisplay {
    public:
        LCDDisplay();
        ~LCDDisplay();
    private:
        std::thread lcdThread;
        bool isRunning;
        UWORD *s_fb;
};

#endif