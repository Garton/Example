/*
 * BitArray.h
 *
 *  Created on: 30 џэт. 2019 у.
 *      Author: Anton
 */

#ifndef BITARRAY_H_
#define BITARRAY_H_
#include "stdint.h"
#include "string.h"
#include "assert.h"
#define DEBUG_BIT_ARRAY

#ifdef DEBUG_BIT_ARRAY
#include "Arduino.h"
#endif

template<size_t __size__>
class BitArray {
	uint8_t array_[__size__];
public:
	BitArray(uint8_t initValue = 0) {
		assert(__size__ != 0);
		if (initValue != 0) {
			initValue = 0xFFFF;
		}
		for (uint8_t i = 0; i < __size__; ++i) {
			array_[i] = initValue;
		}
	}

	bool setBit(const uint8_t btIdx, uint8_t value) {
		if (btIdx < (__size__ * 8)) {
			const uint8_t uiByteIdx = btIdx / 8;
			const uint8_t uiBitIdx = btIdx % 8;
			if (value != 0) {
				array_[uiByteIdx] |= 0x1 << uiBitIdx;
			} else {
				array_[uiByteIdx] &= ~(0x1 << uiBitIdx);
			}
			return true;
		}
		return false;
	}

	bool getBit(uint8_t & retBuffet, const uint8_t btIdx) const {
		if (btIdx < (__size__ * 8)) {
			const uint8_t uiByteIdx = btIdx / 8;
			const uint8_t uiBitIdx = btIdx % 8;
			retBuffet = (array_[uiByteIdx] >> uiBitIdx) & 0x1;
			return true;
		}
		return false;
	}

	bool getByte(uint8_t & retBuffet, const uint8_t bIdx) const {
		if (bIdx < __size__) {
			retBuffet = array_[bIdx];
			return true;
		}
		return false;
	}

	bool setByte(uint8_t value, const uint8_t bIdx) {
		if (bIdx < __size__) {
			array_[bIdx] = value;
			return true;
		}
		return false;
	}

	bool setArray(uint8_t * valArr, const uint8_t uiBSize) {
		if (uiBSize <= __size__) {
			for (uint8_t i = 0; i < uiBSize; ++i) {
				array_[i] = valArr[i];
			}
		}
		return false;
	}

	void setAll(uint8_t value) {
		if (value != 0) {
			value = 0xFFFF;
		}
		for (uint8_t i = 0; i < __size__; ++i) {
			array_[i] = value;
		}
	}

	size_t getByteSize() const {
		return __size__;
	}
	size_t getBitSize() const {
		return __size__ * 8;
	}

	bool operator==(BitArray<__size__>&rightArray) {
		for (uint8_t i = 0; i < __size__; ++i) {
			if (array_[i] != rightArray.array_[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(BitArray<__size__>&rightArray) {
		return !(this->operator ==(rightArray));
	}

	BitArray<__size__> operator~() {
		BitArray<__size__> retArray;
		for (uint8_t i = 0; i < __size__; ++i) {
			retArray.array_[i] = ~array_[i];
		}
		return retArray;
	}
	BitArray<__size__> operator|(BitArray<__size__> rightArray) {
		BitArray<__size__> retArray;
		for (uint8_t i = 0; i < __size__; ++i) {
			retArray.array_[i] = array_[i] | rightArray.array_[i];
		}
		return retArray;
	}

	BitArray<__size__> operator&(BitArray<__size__> rightArray) {
		BitArray<__size__> retArray;
		for (uint8_t i = 0; i < __size__; ++i) {
			retArray.array_[i] = array_[i] & rightArray.array_[i];
		}
		return retArray;
	}

	uint8_t * copyArray() const {
		uint8_t* pExchangeBuffer = malloc(__size__);
		if (pExchangeBuffer != NULL) {
			memcpy(pExchangeBuffer, array_, __size__);
		}
		return pExchangeBuffer;
	}

#ifdef DEBUG_BIT_ARRAY
	void print() {
		Serial.print(F("BitArray<"));
		Serial.print(__size__);
		Serial.print(F(">:"));

		for (uint8_t i = __size__; i > 0; --i) {
			for (uint8_t j = 8; j > 0; --j) {
				Serial.print((array_[i - 1] >> (j - 1)) & 0x1);
			}
			Serial.print(" ");
		}
	}
	void printBitList() {
		Serial.print(F("BitArray<"));
		Serial.print(__size__);
		Serial.print(F("> BitList:"));

		for (uint8_t i = 0; i < __size__; ++i) {
			for (uint8_t j = 0; j < 8; ++j) {
				if ((array_[i] >> j) & 0x1) {
					Serial.print(i * 8 + j);
					Serial.print("; ");
				}
			}
		}
	}
#endif
};

#endif /* BITARRAY_H_ */
