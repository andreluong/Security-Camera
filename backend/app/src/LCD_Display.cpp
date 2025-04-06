#include "LCD_Display.h"
#include "stdbool.h"

#include "DEV_Config.h"
#include "LCD_1in54.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <stdbool.h>
#include <assert.h>
#include <iostream>

#include "personDetector.h"

#define X_POS 5
#define Y_POS 20
#define Y_OFFSET 20
#define MAX_INFO_LINES 4
#define BACKLIGHT 1023

void LCDDisplay_resetBackground() {
    // Initialize the RAM frame buffer to be blank (white)
    Paint_NewImage(s_fb, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);
}

void LCDDisplay_detectPeople() {
    char* title = "People found";
    int x = (LCD_1IN54_WIDTH - strlen(title) * Font16.Width) / 2;
    Paint_DrawString_EN(x, LCD_1IN54_HEIGHT / 2, title, &Font16, WHITE, BLACK);

    int detected = personDetector::getPeopleDetected();
    char detectedStr[MAX_LENGTH];
    snprintf(detectedStr, MAX_LENGTH, "Detected: %d", detected);
    Paint_DrawString_EN(X_POS, LCD_1IN54_HEIGHT - Y_OFFSET, minMax, &Font16, WHITE, BLACK);
}


LCDDisplay::LCDDisplay() {
    // Module Init
    if(DEV_ModuleInit() != 0){
        DEV_ModuleExit();
        exit(0);
    }
    // LCD Init
    DEV_Delay_ms(2000);
    LCD_1IN54_Init(HORIZONTAL);
    LCD_1IN54_Clear(WHITE);
    LCD_SetBacklight(BACKLIGHT);

    UDOUBLE Imagesize = LCD_1IN54_HEIGHT*LCD_1IN54_WIDTH*2;
    if((s_fb = (UWORD *)malloc(Imagesize)) == NULL) {
        perror("Failed to apply for black memory");
        exit(0);
    }
    isRunning = true;
    LCDDisplay_detectPeople();
}


~LCDDisplay::LCDDisplay() {
    free(s_fb);
    s_fb = NULL;

    isRunning = false;
    lcdThread.join();
    LCD_1IN54_Clear(BLACK);
    DEV_ModuleExit();
    std::cout << "LCD off" << std::endl;
}



