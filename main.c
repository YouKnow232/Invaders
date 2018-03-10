#include "mylib.h"
#include "player.h"
#include "pproj.h"
#include "invader1.h"
#include "splashscreen.h"
#include "winscreen.h"
#include "losescreen.h"

/*
 * Author: Chase Harland
 * Project: SpaceInvaders
 * Last Update: 11/2/13
 */

/*
Overview: Animations?
Variables:
 Player position
 Enemy Array (Enemy struct)
 Enemy speed
 Enemy Direction
 Enemy Projectile position (EProj)
 Player projectile position (PProj)
 Copter position
 Copter alive
 Score
 Lives
Game Logic:
 Input
  Pause
  Reset (game loop nested in while(1) loop)
  Movement (left, right)
  Shoot
 Enemy movement
 Projectile movement
 Collisions
  PProj-enemy collision (enemy kill, score++)
  PProj-copter collision (copter kill, score++)
  EProj-shield collision (Shield damage)
  EProj-player collision (Player kill)
 Create EProj (rng based on enemies alive, created at random enemy at bottom of column)
 Create copter (static rng, rng direction)
*/

int main() {
	SETMODE(BG2_ENABLE | MODE3);
	int score;
	char buffer[8];
	int wins;
	int reset;
	int lost;
	int playerPos;
	int pprojIsAlive;
	point pprojPos;
	//int eprojIsAlive;
	//point eprojPos;
	invader invaderArr[32];
	int invadersAlive;
	int invaderMov;
	int invaderTurn;
	int i, j;
	while(1) {
		score = 0;
		reset = FALSE;
		lost = FALSE;
		playerPos = SCREENWIDTH/2;
		pprojIsAlive = FALSE;
		pprojPos.r = 0;
		pprojPos.c = 0;
		invadersAlive = 32;
		invaderMov = 1;
		invaderTurn = FALSE;
		
		for (i = 0; i < 4; i++) {
			for(j = 0; j < 8; j++) {
				invaderArr[OFFSET(i, j, 8)].pos.r = 20+i*15;
				invaderArr[OFFSET(i, j, 8)].pos.c = 10+j*20;
				invaderArr[OFFSET(i, j, 8)].isAlive = TRUE;
			}
		}
		
		fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, WHITE);
		drawImage3(SCREENHEIGHT/2-SPLASHSCREEN_HEIGHT/2, SCREENWIDTH/2-SPLASHSCREEN_WIDTH/2,
			SPLASHSCREEN_WIDTH, SPLASHSCREEN_HEIGHT, splashscreen);
		while(!KEY_DOWN_NOW(BUTTON_START));
		while(KEY_DOWN_NOW(BUTTON_START));
	
		while(!KEY_DOWN_NOW(BUTTON_SELECT) && !reset) {
			// Input		
			if(KEY_DOWN_NOW(BUTTON_START)) {	// Pause
				drawString(0, 0, "Paused", BLACK);
				while(KEY_DOWN_NOW(BUTTON_START));
				while(!KEY_DOWN_NOW(BUTTON_START));
				while(KEY_DOWN_NOW(BUTTON_START));
			}
			if(KEY_DOWN_NOW(BUTTON_RIGHT)) {	// Move right
				playerPos++;
				if (playerPos >= SCREENWIDTH - PLAYER_WIDTH)
					playerPos = SCREENWIDTH-PLAYER_WIDTH;
			}
			if(KEY_DOWN_NOW(BUTTON_LEFT)) {		// Move left
				playerPos--;
				if (playerPos < 0)
					playerPos = 0;
			}
			if(KEY_DOWN_NOW(BUTTON_A)) {		// Create projectile
				if (!pprojIsAlive) {
					pprojIsAlive = TRUE;
					pprojPos.r = SCREENHEIGHT - PLAYER_HEIGHT - PPROJ_HEIGHT;
					pprojPos.c = playerPos + PLAYER_WIDTH/2 - PPROJ_WIDTH/2;
				}
			}
		
			if(pprojIsAlive) {			// Update projectile position
				pprojPos.r-=2;
				if(pprojPos.r <= 0)
					pprojIsAlive = FALSE;
			}
			for(i = 0; i < 32; i++) {		// Move invaders
				if (invaderArr[i].isAlive) {
					if (!invaderTurn) {
						if (invaderMov > 0) {
							if (invaderArr[i].pos.c + INVADER1_WIDTH >= SCREENWIDTH)
								invaderTurn = TRUE;
						} else {
							if (invaderArr[i].pos.c <= 0)
								invaderTurn = TRUE;
						}
					}
					invaderArr[i].pos.c += invaderMov;
				}
			}
			if (invaderTurn) {			// Detect and change directions
				invaderTurn = FALSE;
				invaderMov = -invaderMov;
				for(i = 0; i < 32; i++) {
					if (invaderArr[i].isAlive) {
						invaderArr[i].pos.r += 2;
						invaderArr[i].pos.c += invaderMov;
						if (invaderArr[i].pos.r + INVADER1_HEIGHT >= SCREENHEIGHT-PLAYER_HEIGHT)
							lost = TRUE;
					}
				}
			}
			if (pprojIsAlive) {			// pproj-invader collision detection
				for (i = 0; i < 32; i++) {
					if (invaderArr[i].isAlive) {
						if (pprojPos.c > invaderArr[i].pos.c) {
							if (pprojPos.c < invaderArr[i].pos.c + INVADER1_WIDTH) {
								if (pprojPos.r < invaderArr[i].pos.r + INVADER1_HEIGHT &&
									pprojPos.r + PPROJ_HEIGHT > invaderArr[i].pos.r) {
									pprojIsAlive = FALSE;
									invaderArr[i].isAlive = FALSE;
									invadersAlive--;
									score += 10;
									break;
								}
							}
						} else {
							if (pprojPos.c + PPROJ_WIDTH > invaderArr[i].pos.c) {
								if (pprojPos.r < invaderArr[i].pos.r + INVADER1_HEIGHT &&
									pprojPos.r + PPROJ_HEIGHT > invaderArr[i].pos.r) {
									pprojIsAlive = FALSE;
									invaderArr[i].isAlive = FALSE;
									invadersAlive--;
									score += 10;
									break;
								}
							}
						}
					}
				}
			}

			// Draw functions
			waitForVBlank();
			fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, WHITE);
			for (i = 0; i < 32; i++) {
				if (invaderArr[i].isAlive)
					drawImage3(invaderArr[i].pos.r, invaderArr[i].pos.c, INVADER1_WIDTH, INVADER1_HEIGHT, invader1);
			}
			if(pprojIsAlive == TRUE) {
				drawImage3(pprojPos.r, pprojPos.c, PPROJ_WIDTH, PPROJ_HEIGHT, pproj);
			}
			drawImage3(SCREENHEIGHT-PLAYER_HEIGHT, playerPos, PLAYER_WIDTH, PLAYER_HEIGHT, player);
			sprintf(buffer, "%d", score);
			drawString(0, SCREENWIDTH/2, buffer, BLACK);
			drawString(0, SCREENWIDTH/2+30, "WINS:", BLACK);
			sprintf(buffer, "%d", wins);
			drawString(0, SCREENWIDTH/2+70, buffer, BLACK);
			
			if (invadersAlive <= 0) {		// Win condition
				fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, WHITE);
				drawImage3(SCREENHEIGHT/2-WINSCREEN_HEIGHT/2, SCREENWIDTH/2-WINSCREEN_WIDTH/2,
					WINSCREEN_WIDTH, WINSCREEN_HEIGHT, winscreen);
				while(!KEY_DOWN_NOW(BUTTON_START));
				while(KEY_DOWN_NOW(BUTTON_START));
				wins++;
				reset = TRUE;
			}
			if (lost) {				// Lose condition
				fillRect(0, 0, SCREENWIDTH, SCREENHEIGHT, BLACK);
				drawImage3(SCREENHEIGHT/2-LOSESCREEN_HEIGHT/2, SCREENWIDTH/2-LOSESCREEN_WIDTH/2,
					LOSESCREEN_WIDTH, LOSESCREEN_HEIGHT, losescreen);
				while(!KEY_DOWN_NOW(BUTTON_START));
				while(KEY_DOWN_NOW(BUTTON_START));
				reset = TRUE;
			}
		}
	}
	return 0;
}

void delay(int n) {
	volatile int x = 0;
	for (int i = 0; i < n*1000; i++)
		x++;
	return;
}
