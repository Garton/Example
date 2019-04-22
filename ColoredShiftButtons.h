/*
 * ColoredShiftButtons.h
 *
 *  Created on: 30 џэт. 2019 у.
 *      Author: Anton
 */

#ifndef COLOREDSHIFTBUTTONS_H_
#define COLOREDSHIFTBUTTONS_H_
#include "BitArray.h"
#include "Saver.h"

struct GameButton {
	uint8_t input;
	uint8_t output;
};

template<uint8_t _BSize_>
class ColShButtons: virtual public Saver {
	const GameButton * _gameBtns;
	const uint8_t _uiBtnsPin;
	const uint8_t _uiLedsPin;

	BitArray<_BSize_> buttons;
	BitArray<_BSize_> leds;

	uint8_t _uiCurrentBtnsNumber;
	uint8_t _uiMaxBtnsNumber;
protected:
	uint8_t initSize() {
		uint8_t size = sizeof(_uiCurrentBtnsNumber);
		size += sizeof(_uiMaxBtnsNumber);
		return size;
	}

public:
	ColShButtons(const uint8_t uiBtnsPin, const uint8_t uiLedsPin,
			const GameButton * gameBtns) :
			_uiBtnsPin(uiBtnsPin), _uiLedsPin(uiLedsPin), _gameBtns(gameBtns) {
		_uiCurrentBtnsNumber = 0;
		_uiMaxBtnsNumber = 0;
	}

	bool begin(const uint8_t uiCurrentBtnsNumber = 0,
			const uint8_t uiMaxBtnsNumber = 0) {
		_uiCurrentBtnsNumber = uiCurrentBtnsNumber;
		_uiMaxBtnsNumber = uiMaxBtnsNumber;

		SPI.begin();
		SPI.setClockDivider(SPI_CLOCK_DIV128);
		pinMode(_uiBtnsPin, OUTPUT);
		pinMode(_uiLedsPin, OUTPUT);
		digitalWrite(_uiBtnsPin, LOW);
		digitalWrite(_uiLedsPin, HIGH);

		leds.setAll(0);
		displayLeds();
		return Saver::init();
	}
	const BitArray<_BSize_> getLeds() const {
		return leds;
	}
	void setLeds(const BitArray<_BSize_> & newLeds) {
		leds = newLeds;
	}

	void displayLeds() {
		uint8_t* pExchangeBuffer = leds.copyArray();

		digitalWrite(_uiBtnsPin, HIGH);
		digitalWrite(_uiLedsPin, LOW);
		SPI.transfer(pExchangeBuffer, _BSize_);
		digitalWrite(_uiBtnsPin, LOW);
		digitalWrite(_uiLedsPin, HIGH);
		buttons.setArray(pExchangeBuffer, _BSize_);

		free(pExchangeBuffer);
	}

	void readPushedButtons() {
		displayLeds();
	}

	bool findGameBtnIdx(uint8_t & gameBtnIdx, const uint8_t btnIdx) {

		for (uint8_t i = 0; i < _uiMaxBtnsNumber; ++i) {
			if (_gameBtns[i].input == btnIdx) {
				gameBtnIdx = i;
				return true;
			}
		}
		return false;
	}
	bool findLedBtnIdx(uint8_t & gameBtnIdx, const uint8_t ledIdx) {

		for (uint8_t i = 0; i < _uiMaxBtnsNumber; ++i) {
			if (_gameBtns[i].output == ledIdx) {
				gameBtnIdx = i;
				return true;
			}
		}
		return false;
	}

	void convertButtonsToLeds() {
		leds.setAll(0);
		for (uint8_t btnIdx = 0; btnIdx < (_BSize_ * 8); ++btnIdx) {
			uint8_t uiBtnState = 0;
			if (buttons.getBit(uiBtnState, btnIdx) && uiBtnState) {
				uint8_t gameBtnIdx = 0;
				if (findGameBtnIdx(gameBtnIdx, btnIdx)) {
					leds.setBit(_gameBtns[gameBtnIdx].output, 1);
				}
			}
		}
	}

	void testHandler() {
		readPushedButtons();
		convertButtonsToLeds();
		displayLeds();
	}

	void loadSettings() {
		readData(0, _uiCurrentBtnsNumber);
		readData(1, _uiMaxBtnsNumber);
		if (_uiMaxBtnsNumber > (_BSize_ * 8)) {
			_uiMaxBtnsNumber = _BSize_ * 8;
		}
		if (_uiCurrentBtnsNumber > _uiMaxBtnsNumber) {
			_uiCurrentBtnsNumber = _uiMaxBtnsNumber;
		}
	}

	void setCurrentBtnsNumber(const uint8_t uiCurrentBtnsNumber = 0) {
		if (uiCurrentBtnsNumber < _uiMaxBtnsNumber) {
			_uiCurrentBtnsNumber = uiCurrentBtnsNumber;
		} else {
			_uiCurrentBtnsNumber = _uiMaxBtnsNumber;
		}
	}
	uint8_t getCurrentBtnsNumber() const {
		return _uiCurrentBtnsNumber;
	}

	void setMaxBtnsNumber(const uint8_t uiMaxBtnsNumber = 0) {
		_uiMaxBtnsNumber = uiMaxBtnsNumber;
		if (_uiCurrentBtnsNumber > _uiMaxBtnsNumber) {
			_uiCurrentBtnsNumber = _uiMaxBtnsNumber;
		}
	}

	uint8_t getMaxBtnsNumber() const {
		return _uiMaxBtnsNumber;
	}

	void saveSettings() {
		saveData(0, _uiCurrentBtnsNumber);
		saveData(1, _uiMaxBtnsNumber);
	}

	void lightUpRandButtons() {
		leds.setAll(0);
		Serial.print("curr:");
		Serial.println(_uiCurrentBtnsNumber);
		Serial.print("max:");
		Serial.println(_uiMaxBtnsNumber);
		for (uint8_t btnIdx = 0; btnIdx < _uiCurrentBtnsNumber; ++btnIdx) {
			uint8_t ledNum = random(0, _uiMaxBtnsNumber);
			uint8_t ledState = 0;
			while (leds.getBit(ledState, _gameBtns[ledNum].output) && ledState) {
				ledNum = random(0, _uiMaxBtnsNumber);
			}
			leds.setBit(_gameBtns[ledNum].output, 1);
		}
		displayLeds();
	}

	void lightOffAllButtons() {
		leds.setAll(0);
		displayLeds();
	}

	uint8_t getColoredBtnsNumber() const {
		uint8_t uiColoredBtns = 0;
		for (uint8_t ledIdx = 0; ledIdx < (_BSize_ * 8); ++ledIdx) {
			uint8_t uiLedState = 0;
			if (leds.getBit(uiLedState, ledIdx) && uiLedState) {
				++uiColoredBtns;
			}
		}
		return uiColoredBtns;
	}

	uint8_t colorBtnsPushedNumber() {
		displayLeds();
		uint8_t colorBtnsPushed = 0;
		for (uint8_t ledIdx = 0; ledIdx < (_BSize_ * 8); ++ledIdx) {
			uint8_t uiLedState = 0;
			if (leds.getBit(uiLedState, ledIdx) && uiLedState) {
				uint8_t uGameBtnIdx = 0;
				if (findLedBtnIdx(uGameBtnIdx, ledIdx)) {
					uint8_t uiBtnState = 0;
					if (buttons.getBit(uiBtnState, _gameBtns[uGameBtnIdx].input)
							&& uiBtnState) {
						colorBtnsPushed++;
					}
				}
			}
		}
		return colorBtnsPushed;
	}
	bool colorBtnPushed() {
		displayLeds();
		for (uint8_t ledIdx = 0; ledIdx < (_BSize_ * 8); ++ledIdx) {
			uint8_t uiLedState = 0;
			if (leds.getBit(uiLedState, ledIdx) && uiLedState) {
				uint8_t uGameBtnIdx = 0;
				if (findLedBtnIdx(uGameBtnIdx, ledIdx)) {
					uint8_t uiBtnState = 0;
					if (buttons.getBit(uiBtnState, _gameBtns[uGameBtnIdx].input)
							&& uiBtnState) {
						leds.setBit(ledIdx, 0);
						displayLeds();
						return true;
					}
				}
			}
		}
		return false;
	}
};

#endif /* COLOREDSHIFTBUTTONS_H_ */
