/*
 * Game.h
 *
 *  Created on: 30 џэт. 2019 у.
 *      Author: Anton
 */

#ifndef GAME_H_
#define GAME_H_
#include <Button.h>
#include "TM/TMQuadDisplay.h"
#include "Spi.h"
#include "ColoredShiftButtons.h"
#include "DFPlayer_Mini_Mp3.h"
#include "CTimer.h"
#include "BitArray.h"
#define GAME_REGISTERS_NUMBER 3
#define GAME_BUTTONS_NUMBER 20

extern TM_QuadDisplay tmTimerDisplay;
extern TM_QuadDisplay tmSettingsDisplay;
extern CTImer cTimer;


extern Button btnNewRound;
extern Button btnMenuSave;
extern Button btnSettings;
extern Button btnPlus;
extern Button btnMinus;


extern BitArray<GAME_REGISTERS_NUMBER> GamePuttonsLeds;


extern ColShButtons<GAME_REGISTERS_NUMBER> coloredBtns;

void gameHandler();

#endif /* GAME_H_ */
