#include <Arduino.h>
#include "Game.h"
//#define DEBUG_SETTINGS_DISPLAY
//#define DEBUG_TIMER_DISPLAY
//#define DEBUG_ADMIN_BUTTONS
//#define DEBUG_LIGHT_ALL
//#define DEBUG_BUTTONS_FINDER
#ifdef DEBUG_BUTTONS_FINDER
#include "ButtonsFinder/ButtonsFinder.h"
#endif
enum ports
	:uint8_t {
		PLAYER_NBUZY = 2, BUZZ = 4,

		BTN_NEW_ROUND = 5, BTN_MENU_SAVE = 6, BTN_SETTINGS = 7,

		BTN_PLUS = 14, BTN_MINUS = 15,
		//////////////SETTINGS DISPLAY////////////////
	DISP_SETTINGS_DATA = 8,
	DISP_SETTINGS_CLOCK = 9,

	//////////////TIMER DISPLAY////////////////
	DISP_TIMER_DATA = A2,
	DISP_TIMER_CLOCK = A3,

	///////////////
	//REG_DATA_PIN = 12,
	//REG_CLOCK_PIN = 13,
	REGIN_ENABLE_PIN = 10,
	REGOUT_ENABLE_PIN = A4,
};

Button btnNewRound(BTN_NEW_ROUND, 20);
Button btnMenuSave(BTN_MENU_SAVE, 20);
Button btnSettings(BTN_SETTINGS, 20, 2000);
Button btnPlus(BTN_PLUS, 20);
Button btnMinus(BTN_MINUS, 20);

void testAdminButtons();
void debugLightAll();
////////////////////DISPLAYS////////////////////////
void testTMQuadDisplayText(TM_QuadDisplay & tmQD);

//////////////SETTINGS DISPLAY////////////////
TM_QuadDisplay tmSettingsDisplay;
void beginSettingDisplay();

//////////////TIMER DISPLAY///////////////////////
TM_QuadDisplay tmTimerDisplay;
CTImer cTimer(tmTimerDisplay);
void beginTimerDisplay();

////////////////////GAME BUTTONS///////////////////

const GameButton gameButtons[GAME_BUTTONS_NUMBER] = { { 0, 7 }, { 1, 6 },
		{ 2, 5 }, { 3, 4 }, { 4, 3 }, { 5, 2 }, { 6, 1 }, { 7, 15 }, { 8, 14 },
		{ 9, 13 }, //
		///NON ACTIVE
		{ 10, 12 }, { 11, 11 }, { 12, 10 }, { 13, 9 }, { 14, 23 }, { 15, 22 }, {
				16, 21 }, { 17, 20 }, { 18, 19 }, { 19, 18 }, };

ColShButtons<GAME_REGISTERS_NUMBER> coloredBtns(REGIN_ENABLE_PIN,
		REGOUT_ENABLE_PIN, gameButtons);

void setup() {
	Serial.begin(9600);
	mp3_set_serial(Serial);

	beginSettingDisplay();
	beginTimerDisplay();

	tmQD_Display(tmSettingsDisplay, "----");

	if (!coloredBtns.begin(20, 20)) {
		Serial.println(F("ColShButtons begin ERROR!"));
	}

	coloredBtns.loadSettings();
	cTimer.begin(coloredBtns.getCurrentBtnsNumber());
	cTimer.loadSettings();
}

void loop() {
#ifdef DEBUG_SETTINGS_DISPLAY
	testTMQuadDisplayText(tmSettingsDisplay);
#define DEBUG
#endif
#ifdef DEBUG_TIMER_DISPLAY
	testTMQuadDisplayText(tmTimerDisplay);
#define DEBUG
#endif
#ifdef DEBUG_ADMIN_BUTTONS
	testAdminButtons();
#define DEBUG
#endif

#ifdef DEBUG_LIGHT_ALL
	debugLightAll();
#define DEBUG
#endif
#ifdef DEBUG_BUTTONS_FINDER
	searchButtons(REGIN_ENABLE_PIN);
	searchLeds(REGOUT_ENABLE_PIN);
#define DEBUG
#endif

#ifndef DEBUG
	gameHandler();
#endif
}

void testAdminButtons() {
	uint16_t time = millis();
	btnNewRound.check(time);
	btnMenuSave.check(time);
	btnSettings.check(time);
	btnPlus.check(time);
	btnMinus.check(time);

	if (btnNewRound.oneClickShort()) {
		Serial.println(F("btnNewRound Click"));
	}
	if (btnMenuSave.oneClickShort()) {
		Serial.println(F("btnMenuSave Click"));
	}
	if (btnSettings.oneClickShort()) {
		Serial.println(F("btnSettings Click"));
	}
	if (btnPlus.oneClickShort()) {
		Serial.println(F("btnPlus Click"));
	}
	if (btnMinus.oneClickShort()) {
		Serial.println(F("btnMinus Click"));
	}
}
void beginSettingDisplay() {
	tmSettingsDisplay.tm.clock_pin = DISP_SETTINGS_CLOCK;
	tmSettingsDisplay.tm.data_pin = DISP_SETTINGS_DATA;
	tmSettingsDisplay.tm.bright = TM_MAX_BRIGHTNESS;
	tmSettingsDisplay.effects.uiStrDir = TM_QD_LEFT_DIR;
	tmSettingsDisplay.effects.uiTickerPeriod_ms = 500;
	tmSettingsDisplay.effects.bTicker = true;
	tmQD_Init(tmSettingsDisplay);
}

void beginTimerDisplay() {
	tmTimerDisplay.tm.clock_pin = DISP_TIMER_CLOCK;
	tmTimerDisplay.tm.data_pin = DISP_TIMER_DATA;
	tmTimerDisplay.tm.bright = TM_MAX_BRIGHTNESS;
	tmTimerDisplay.effects.uiStrDir = TM_QD_LEFT_DIR;
	tmTimerDisplay.effects.uiTickerPeriod_ms = 500;
	tmTimerDisplay.effects.bTicker = true;
	tmQD_Init(tmTimerDisplay);
}

void testTMQuadDisplayText(TM_QuadDisplay & tmQD) {
	tmQD_Display_Handler(tmQD);
	if (Serial.available()) {
		String strMsg = Serial.readString();
		if (strMsg.length() > 0) {
			strMsg[strMsg.length() - 1] = '\0';
		}
		Serial.println(strMsg);
		tmQD_Display(tmQD, strMsg.c_str());
	}
}

void debugLightAll() {
	uint8_t uiBuf[GAME_REGISTERS_NUMBER];
	memset(uiBuf, 0xFF, GAME_REGISTERS_NUMBER);
	digitalWrite(REGOUT_ENABLE_PIN, HIGH);
	SPI.transfer(uiBuf, GAME_REGISTERS_NUMBER);
	digitalWrite(REGOUT_ENABLE_PIN, LOW);
}
