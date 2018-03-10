#include "mylib.h"

/*
 * Author: Chase Harland
 * Project: SpaceInvaders
 * Last Update: 11/2/13
 */


u16 *videoBuffer = (u16*) 0x6000000;

void setPixel(int r, int c, u16 color) {
	videoBuffer[OFFSET(r, c, SCREENWIDTH)] = color;
}

void fillRect(int r, int c, int width, int height, u16 color) {
	int i;
	for (i = 0; i < height; i++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(r+i, c, SCREENWIDTH)];
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void drawImage3(int r, int c, int width, int height, const u16* image) {
	int i;
	for (i = 0; i < height; i++) {
		DMA[3].src = &image[width*i];
		DMA[3].dst = &videoBuffer[OFFSET(r+i, c, SCREENWIDTH)];
		DMA[3].cnt = width | DMA_ON;
	}
}

void drawChar(int r, int c, char character, u16 color) {
	int i, j;
	for (i = 0; i<8; i++) {
		for(j = 0; j<6; j++) {
			if(fontdata_6x8[OFFSET(i, j, 6)+48*character]) {
				setPixel(i+r, j+c, color);
			}
		}
	}
}

void drawString(int r, int c, char *character, u16 color) {
	while(*character) {
		drawChar(r, c, *character++, color);
		c += 6;
	}
}

void waitForVBlank() {
	while(SCANLINECOUNTER > SCREENHEIGHT);
	while(SCANLINECOUNTER < SCREENHEIGHT);
}
