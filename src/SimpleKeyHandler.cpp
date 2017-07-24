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
 */

#include "SimpleKeyHandler.h"

SimpleKeyHandler::SimpleKeyHandler() {
	clear();
	_companion = nullptr;
	_nextValidRead = 0;
	_previousState = keyOff;
	_count = 0;
}
/*
 * Clears all the callback pointer;
 */
void SimpleKeyHandler::clear() {
	onShortPress = nullptr;
	onLongPress = nullptr;
	onBothPress = nullptr;
	onRepPress = nullptr;
	onRepPressCount = nullptr;
}

/*
 * To be placed in the main loop. Expect TRUE if a key is pressed.
 */
void SimpleKeyHandler::read(bool keyState) {
	switch (_previousState) {
	case keyOff:
		if (keyState) {
			// when on advance to the next state
			_previousState = keyToOn;
			_nextValidRead = millis() + debounce;
		}
		break;
	case keyToOn:
		// ignore the key until debounce time expired
		if (millis() >= _nextValidRead) {
			if (keyState) {
				// when still on advance to the next state
				_previousState = keyOn;
				_nextValidRead = millis() + longPress;
			} else
				// otherwise it was a glitch
				_previousState = keyOff;
		}
		break;
	case keyOn:
		if (!keyState) {
			// when off advance to the next state
			_previousState = keyToOff;
			_nextValidRead = millis() + debounce;
		} else {
			// callback after long press and repeat after the repeat interval
			if (millis() >= _nextValidRead) {
				_nextValidRead = millis() + repeatInterval;
				if (onLongPress && _count == 0)
					onLongPress();
				if (onRepPressCount)
					onRepPressCount(_count);
				if (onRepPress)
					onRepPress();
				_count++;
			} else if (_count == 0 && _companion
					&& _companion->_previousState == keyOn
					&& _companion->_count == 0) {
				if (onBothPress)
					onBothPress();
				else if (_companion->onBothPress)
					_companion->onBothPress();
				// prevent each key to callback shortpress
				_count++;
				_companion->_count++;
			}
		}
		break;
	case keyToOff:
		// ignore the key until debounce time expired
		if (millis() >= _nextValidRead) {
			if (!keyState) {
				// when off advance to the next state
				_previousState = keyOff;
				// if key was released within the long press time callback
				if (onShortPress && _count == 0)
					onShortPress();
				else
					_count = 0;
			} else
				// otherwise it was a glitch
				_previousState = keyOn;
		}
		break;
	}
}

/*
 * Checks if the key is in the on stage
 */
bool SimpleKeyHandler::isPressed() {
	return _previousState == keyOn;
}

/*
 * Couples a companion key. When both keys are pressed the
 * onBothPressed is executed. Only one key has to be coupled
 */
void SimpleKeyHandler::setCompanion(SimpleKeyHandler* companion) {
	if (companion) {
		if (_companion) {
			_companion->_companion = nullptr;
		}
		_companion = companion;
		_companion->_companion = this;
	} else {
		if (_companion) {
			_companion->_companion = nullptr;
		}
		_companion = companion;
	}
}
