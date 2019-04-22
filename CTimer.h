/*
 * CTimer.h
 *
 *  Created on: 30 џэт. 2019 у.
 *      Author: Anton
 */

#ifndef CTIMER_H_
#define CTIMER_H_
#include "Saver.h"
#include "TM/TMQuadDisplay.h"

class CTImer: public virtual Saver {
	TM_QuadDisplay & _tmDisplay;
	uint16_t _uiTimerPeriod_s;
	uint16_t _uiTimerFullPeriod_s;
	uint8_t _playersNumber;

	uint32_t _uiOldTime_ms;
	bool started = false;
protected:
	uint8_t initSize() {
		uint8_t size = sizeof(_uiTimerPeriod_s);
		return size;
	}

public:
	CTImer(TM_QuadDisplay & tmDisplay) :
			_tmDisplay(tmDisplay) {
		_uiTimerPeriod_s = 0;
		_uiTimerFullPeriod_s = 0;
		_uiOldTime_ms = 0;
		_playersNumber = 0;
	}
	bool begin(const uint8_t playersNumber) {
		_playersNumber = playersNumber;
		Serial.print(F("_playersNumber"));
		Serial.println(_playersNumber);
		return Saver::init();
	}

	void print(const uint16_t& uiCurrentTime_s) {
		tmQD_DisplayUInt16(_tmDisplay, uiCurrentTime_s);
		tmQD_Display_Handler(_tmDisplay);
	}

	bool check(uint32_t& time) {
		if (started) {
			if (_uiOldTime_ms != time) {
				uint16_t uiCurrentTime_s = (time - _uiOldTime_ms) / 1000;
				if (uiCurrentTime_s > _uiTimerFullPeriod_s) {
					return true;
				}
				print(_uiTimerFullPeriod_s - uiCurrentTime_s);
			}
		}

		return false;
	}
	void refresh() {
		_uiOldTime_ms = millis();
	}
	bool check() {
		if (started) {
			uint32_t time = millis();
			return check(time);
		}
		return false;
	}
	bool start(uint32_t& time) {
		if (!started) {
			started = true;
			_uiOldTime_ms = time;
			return true;
		}
		return false;
	}
	bool start() {
		if (!started) {
			uint32_t time = millis();
			return start(time);
		}
		return false;
	}
	void stop() {
		started = false;
	}
	void saveSettings() {
		saveData(0, _uiTimerPeriod_s);
	}
	void loadSettings() {
		readData(0, _uiTimerPeriod_s);
		if (_uiTimerPeriod_s > 300) {
			_uiTimerPeriod_s = 300;
			saveSettings();
		}
		_uiTimerFullPeriod_s = _uiTimerPeriod_s * _playersNumber;
		Serial.print(F("_uiTimerFullPeriod_s="));
		Serial.println(_uiTimerFullPeriod_s);
	}

	uint16_t getTimePeriod() const {
		return _uiTimerPeriod_s;
	}
	void setTimePeriod(uint16_t &uiTimerPeriod_s) {
		_uiTimerPeriod_s = uiTimerPeriod_s;
		_uiTimerFullPeriod_s = _uiTimerPeriod_s * _playersNumber;
	}

	void setPlayersNumber(const uint8_t playersNumber) {
		_playersNumber = playersNumber;
		_uiTimerFullPeriod_s = _uiTimerPeriod_s * _playersNumber;
		Serial.print(F("_uiTimerFullPeriod_s"));
		Serial.println(_uiTimerFullPeriod_s);
	}
};

#endif /* CTIMER_H_ */
