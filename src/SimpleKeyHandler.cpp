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

/*
 * These variables and function pointer are defined as static as they
 * are common for all instances of this class
 */
uint16_t SimpleKeyHandler::_count = 0;
SimpleKeyHandler* SimpleKeyHandler::_activeKey = nullptr;
SimpleKeyHandler* SimpleKeyHandler::_otherKey = nullptr;
void (*SimpleKeyHandler::onTwoPress)(const SimpleKeyHandler* senderKey,
		const SimpleKeyHandler* otherKey) = nullptr;

SimpleKeyHandler::SimpleKeyHandler() {
	clear();
	_nextValidRead = 0;
	_previousState = keyOff;
	_allowEvents = false;
}
/*
 * Clears all the callback pointers of the key (not onTwopress).
 */
void SimpleKeyHandler::clear() {
	onShortPress = nullptr;
	onLongPress = nullptr;
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
				// disable the other keys
				if (!_activeKey)
					_activeKey = this;
				// try to claim the other key
				else if (!_otherKey)
					_otherKey = this;
				_allowEvents = (_activeKey == this);
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
				// prevent events when disabled
				if ((_allowEvents)) {
					if (onLongPress && _count == 0)
						onLongPress();
					if (onRepPressCount)
						onRepPressCount(_count);
					if (onRepPress)
						onRepPress();
					_count++;
				}
			} else {
				// handle the two key press;
				if (_allowEvents && _otherKey && _count == 0) {
					if (onTwoPress) {
						onTwoPress(this, _otherKey);
						// this is the only callback we do
						_allowEvents = false;
					}
				}
			}
		}
		break;
	case keyToOff:
		// ignore the key until debounce time expired
		if (millis() >= _nextValidRead) {
			if (!keyState) {
				// when off advance to the next state
				_previousState = keyOff;
				if (_allowEvents && onShortPress && _count == 0)
					// if key was released within the long press time callback
					onShortPress();
				// clean up if active key
				if (_activeKey == this) {
					_count = 0;
					_activeKey = nullptr;
					_otherKey = nullptr;
				}
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
