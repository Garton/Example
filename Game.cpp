/*
 * Game.cpp
 *
 *  Created on: 30 џэт. 2019 у.
 *      Author: Anton
 */
#include "Game.h"
enum GameSound
	:uint8_t {
		StartSound = 0, GameOverSound = 1, FinishSound = 2, StartTimerSound = 3,

		ButtonsSound = 10,
};

enum GameState
	:uint8_t {
		GameStoped, GameStarted, GameSettings,
} gameState(GameStoped);

enum MainMenu
	:uint8_t {
		thisMenu, BeginRound
} showMainMenu(BeginRound), selectedMainMenu(thisMenu);

BitArray<GAME_REGISTERS_NUMBER> GameButtonsLeds(0);

void gameStopedMenu();
void gameStartedMenu();
void gameSettingsMenu();

void gameHandler() {
	uint16_t time = millis();
	btnNewRound.check(time);
	btnMenuSave.check(time);
	btnSettings.check(time);
	btnPlus.check(time);
	btnMinus.check(time);

	tmQD_Display_Handler(tmSettingsDisplay);

	switch (gameState) {
	case GameStoped:
		gameStopedMenu();
		break;
	case GameStarted:
		gameStartedMenu();
		break;
	case GameSettings:
		gameSettingsMenu();
		break;
	}
}

uint8_t uiSettingsCurrent = -1;
void gameSettingsMenu() {

	if (btnMenuSave.oneClickShort()) {
		uiSettingsCurrent = -1;
		cTimer.saveSettings();
		coloredBtns.saveSettings();
		gameState = GameStoped;
		tmQD_Display(tmSettingsDisplay, "----");
		Serial.println("GameStoped");
		return;
	}

	if (btnSettings.oneClickLong()) {
		uiSettingsCurrent = -1;
		gameState = GameStoped;
		tmQD_Display(tmSettingsDisplay, "----");
		Serial.println("GameStoped");
		return;
	}
	uint16_t uiCurrentTPer = cTimer.getTimePeriod();
	uint8_t uiPlayersMaxNumber = coloredBtns.getMaxBtnsNumber();
	switch (uiSettingsCurrent) {
	case 0: {
		if (btnSettings.oneClickShort()) {
			uiSettingsCurrent = 1;
			tmQD_Display(tmSettingsDisplay, "b");
		}

		static uint16_t uiPlusTime = millis();
		static uint16_t uiMinusTime = millis();
		if (btnPlus.get()) {
			uint16_t time_ms = millis();
			uiMinusTime = time_ms;
			if (btnPlus.oneClickShort()) {
				uiPlusTime = time_ms;
				if (uiCurrentTPer < 300) {
					tmQD_DisplayUInt16(tmSettingsDisplay, ++uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
			}
			if ((time_ms - uiPlusTime) > 10000) {
				if (uiCurrentTPer < 300) {
					if ((300 - uiCurrentTPer) > 100) {
						uiCurrentTPer += 100;
					} else {
						uiCurrentTPer = 300;
					}
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
				delay(50);
			} else if ((time_ms - uiPlusTime) > 6000) {
				if (uiCurrentTPer < 300) {
					if ((300 - uiCurrentTPer) > 10) {
						uiCurrentTPer += 10;
					} else {
						uiCurrentTPer = 300;
					}
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
				delay(50);
			}
		} else if (btnMinus.get()) {
			uint16_t time_ms = millis();
			uiPlusTime = time_ms;
			if (btnMinus.oneClickShort()) {
				uiMinusTime = time_ms;
				if (uiCurrentTPer > 1) {
					tmQD_DisplayUInt16(tmSettingsDisplay, --uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
			}
			if ((time_ms - uiMinusTime) > 10000) {
				if (uiCurrentTPer > 1) {
					if (uiCurrentTPer > 101) {
						uiCurrentTPer -= 100;
					} else {
						uiCurrentTPer = 1;
					}
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
				delay(50);
			} else if ((time_ms - uiMinusTime) > 6000) {
				if (uiCurrentTPer > 1) {
					if (uiCurrentTPer > 10) {
						uiCurrentTPer -= 10;
					} else {
						uiCurrentTPer = 1;
					}
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
					cTimer.setTimePeriod(uiCurrentTPer);
				} else {
					tmQD_DisplayUInt16(tmSettingsDisplay, uiCurrentTPer);
				}
				delay(50);
			}
		}

	}
		break;
	case 1:
		if (btnSettings.oneClickShort()) {
			uiSettingsCurrent = 0;
			tmQD_Display(tmSettingsDisplay, "P");
		}
		if (btnPlus.oneClickShort()) {
			uiPlayersMaxNumber = 20;
			tmQD_DisplayUInt16(tmSettingsDisplay, uiPlayersMaxNumber);
			coloredBtns.setMaxBtnsNumber(uiPlayersMaxNumber);
		} else if (btnMinus.oneClickShort()) {
			uiPlayersMaxNumber = 10;
			tmQD_DisplayUInt16(tmSettingsDisplay, uiPlayersMaxNumber);
			coloredBtns.setMaxBtnsNumber(uiPlayersMaxNumber);
		}
		break;
	default:
		uiSettingsCurrent = 0;
		tmQD_Display(tmSettingsDisplay, "P");
	}

}

void clearDisplays() {
	tmQD_Display(tmSettingsDisplay, "----");
	tmQD_Display_Handler(tmSettingsDisplay);
	tmQD_Display(tmTimerDisplay, "----");
	tmQD_Display_Handler(tmTimerDisplay);
}
void startGame() {
	coloredBtns.saveSettings();
	coloredBtns.lightUpRandButtons();
	Serial.flush();
	mp3_play(StartSound);
	cTimer.refresh();
	clearDisplays();
	GameButtonsLeds = coloredBtns.getLeds();
	gameState = GameStarted;
	Serial.println("GameStarted");
}

void gameStopedMenu() {
	if (btnNewRound.oneClickShort()) {
		Serial.println("btnNewRound");
		startGame();
	}
	if (btnPlus.oneClickShort()) {
		uint8_t currentBtnsNumber = coloredBtns.getCurrentBtnsNumber();
		uint8_t maxBtnsNumber = coloredBtns.getMaxBtnsNumber();
		if (currentBtnsNumber < maxBtnsNumber) {
			tmQD_DisplayUInt8(tmSettingsDisplay, ++currentBtnsNumber);
			coloredBtns.setCurrentBtnsNumber(currentBtnsNumber);
			cTimer.setPlayersNumber(currentBtnsNumber);
		} else {
			tmQD_DisplayUInt8(tmSettingsDisplay, currentBtnsNumber);
		}

	} else if (btnMinus.oneClickShort()) {
		uint8_t currentBtnsNumber = coloredBtns.getCurrentBtnsNumber();
		uint8_t maxBtnsNumber = coloredBtns.getMaxBtnsNumber();
		if (currentBtnsNumber > 1) {
			tmQD_DisplayUInt8(tmSettingsDisplay, --currentBtnsNumber);
			coloredBtns.setCurrentBtnsNumber(currentBtnsNumber);
			cTimer.setPlayersNumber(currentBtnsNumber);
		} else {
			tmQD_DisplayUInt8(tmSettingsDisplay, currentBtnsNumber);
		}
	}
	if (btnSettings.oneClickShort()) {
		gameState = GameSettings;
		Serial.println("GameSettings");
	}
}

void stopGame() {
	mp3_play(GameOverSound);
	coloredBtns.lightOffAllButtons();
	Serial.println("GameStoped");
	tmQD_Display(tmSettingsDisplay, "----");
	cTimer.stop();
	gameState = GameStoped;
}

bool isPazzleSolved() {
	static uint16_t oldTime = millis();
	static bool activated = false;

	uint8_t currentLedBtnPushed = coloredBtns.getColoredBtnsNumber();
	if (currentLedBtnPushed == 0) {
		uint16_t time = millis();
		if (!activated) {
			activated = true;
			oldTime = time;
		} else {
			if ((time - oldTime) > 500) {
				return true;
			}
		}
	} else {
		activated = false;
	}
	return false;
}

void printPushedBtnsNumber() {
	static uint8_t oldPushedNumber = 0;
	uint8_t currentLedBtnPushed = coloredBtns.getCurrentBtnsNumber()
			- coloredBtns.getColoredBtnsNumber();
	if (currentLedBtnPushed != oldPushedNumber) {
		oldPushedNumber = currentLedBtnPushed;
		char * msgAdmin = malloc(5); //4 char + '\0'
		if (msgAdmin != NULL) {
			strcpy(msgAdmin, "Ph");
			if (currentLedBtnPushed < 10) {
				msgAdmin[2] = '0';
				msgAdmin[3] = currentLedBtnPushed + '0';
				msgAdmin[4] = '\0';
				tmQD_Display(tmSettingsDisplay, msgAdmin);
			} else if (currentLedBtnPushed < 100) {
				itoa(currentLedBtnPushed, msgAdmin + 2, 10);
				msgAdmin[4] = '\0';
				tmQD_Display(tmSettingsDisplay, msgAdmin);
			}
			free(msgAdmin);
		}
	}
}

void timerHandler() {
	uint16_t time = millis();

}

void gameStartedMenu() {
	if (btnNewRound.oneClickShort()) {
		stopGame();
		return;
	}
	printPushedBtnsNumber();
	if (coloredBtns.colorBtnPushed()) {
		if (cTimer.start()) {
			Serial.flush();
			mp3_play(StartTimerSound);
		} else {
			Serial.flush();
			mp3_play(ButtonsSound + random(0, 5));
		}
	}
	if (isPazzleSolved()) {
		tmQD_Display(tmSettingsDisplay, "-GEnd-");
		Serial.flush();
		mp3_play(FinishSound);
		cTimer.stop();

		gameState = GameStoped;
	}
	if (cTimer.check()) {
		Serial.flush();
		mp3_play(GameOverSound);
		coloredBtns.setLeds(GameButtonsLeds);
		cTimer.refresh();
		cTimer.stop();
	}
}

