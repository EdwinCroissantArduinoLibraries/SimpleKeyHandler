/*
 * Simple key handler version 0.0.1 (09may2017)
 *
 * Copyright (C) 2017 Edwin Croissant
 *
 *  This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the README.md file for additional information.
 *
 * version history:
 * 00.01 09may2017 First release
 * 00.02 24jul2017 Removed setCompanion and onBothPress, introduced onTwoPress callback
 */

#ifndef SIMPLEKEYHANDLER_H_
#define SIMPLEKEYHANDLER_H_

#include "Arduino.h"


class SimpleKeyHandler {
public:
	SimpleKeyHandler();
	void read(bool keyState);
	void clear();
	bool isPressed();
	// Called when the key is released before the long press time expired.
	void (*onShortPress)();
	// Called when the long press time expired
	void (*onLongPress)();
	// Repeatedly called when the long press time expired
	void (*onRepPress)();
	// Repeatedly called when the long press time expired, the count is passed as an argument
	void (*onRepPressCount)(uint16_t count);
	// Called when two keys are pressed at the same time
	static void (*onTwoPress)(const SimpleKeyHandler* senderKey,
			const SimpleKeyHandler* otherKey);
private:
	enum lastKeyState {
		keyOff, keyToOn, keyOn, keyToOff
	} _previousState;
	enum keyTime {
		debounce = 50,
		longPress = 500,
		repeatInterval = 250
	};
	uint32_t _nextValidRead;
	bool _allowEvents;
	static uint16_t _count;
	static SimpleKeyHandler* _activeKey;
	static SimpleKeyHandler* _otherKey;
};

#endif /* SIMPLEKEYHANDLER_H_ */
