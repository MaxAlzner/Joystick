#ifndef JOYSTICK_H
#define JOYSTICK_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/// <summary>
/// Provides methods for interfacing with a 2-axis joystick and an optional button.
/// </summary>
class Joystick
{
public:

#if !defined(JOYSTICK_LOWP)
	typedef double precision_t;
#else
	typedef float precision_t;
#endif

	/// <param name="x">Pin of the x-axis.</param>
	/// <param name="x">Pin of the y-axis.</param>
	inline Joystick(const byte x, const byte y) :
		_x(x), _y(y), _btn(0xff), _latched(false) {}
	/// <param name="x">Pin of the x-axis.</param>
	/// <param name="x">Pin of the y-axis.</param>
	/// <param name="btn">Pin of the stick's button.</param>
	inline Joystick(const byte x, const byte y, const byte btn) :
		_x(x), _y(y), _btn(btn), _latched(false) {}

	/// <summary>
	/// Initializes the connection to the joysick.
	/// </summary>
	/// <param name="deadzone">Only start reading movement from the stick after the axis has passed this value.</param>
	inline void begin(const precision_t deadzone = 0.1);

	/// <summary>
	/// Reads whether or not the joystick's button is pressed.
	/// </summary>
	/// <param name="latch">Whether or not to suppress any reading of the button after this call.</param>
	inline bool pressed(bool latch = false);
	/// <summary>
	/// Reads the current position (-1 to 1) of the stick on the x-axis.
	/// </summary>
	inline precision_t x();
	/// <summary>
	/// Reads the current position (-1 to 1) of the stick on the y-axis.
	/// </summary>
	inline precision_t y();
	/// <summary>
	/// Reads how far the stick is away from the origin on a 0 to 1 value.
	/// </summary>
	inline precision_t linear();
	/// <summary>
	/// Reads how far the stick is away from the origin.
	/// </summary>
	inline precision_t magnitude();
	/// <summary>
	/// Reads the direction (0 to 360 degrees) the stick is currently pointing.
	/// </summary>
	inline precision_t angle();

protected:

	/// <summary>
	/// Reads and maps a channel to a -1 to 1 value.
	/// </summary>
	inline precision_t _axis(const byte channel);

	byte _x, _y, _btn;
	precision_t _deadzone;
	bool _latched;

};

inline void Joystick::begin(const precision_t deadzone)
{
	this->_deadzone = deadzone;
	if (this->_btn != 0xff)
	{
		pinMode(this->_btn, INPUT);
	}
}

inline Joystick::precision_t Joystick::_axis(const byte channel)
{
	precision_t raw = (((precision_t)analogRead(channel) / (precision_t)1023) * (precision_t)2) - (precision_t)1;
	precision_t value = abs(raw);
	return (value <= this->_deadzone) ? (precision_t)0 : (((value - this->_deadzone) / ((precision_t)1 - this->_deadzone)) * (raw / value));
}

inline bool Joystick::pressed(bool latch)
{
	bool value = (digitalRead(this->_btn) == 0);
	if (value)
	{
		if (latch)
		{
			if (!this->_latched)
			{
				this->_latched = true;
				return true;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	this->_latched = false;
	return false;
}
inline Joystick::precision_t Joystick::x()
{
	return this->_axis(this->_x);
}
inline Joystick::precision_t Joystick::y()
{
	return this->_axis(this->_y);
}
inline Joystick::precision_t Joystick::linear()
{
	return min(this->magnitude(), (precision_t)1);
}
inline Joystick::precision_t Joystick::magnitude()
{
	precision_t _x = this->x(), _y = this->y();
	return sqrt((_x * _x) + (_y * _y));
}
inline Joystick::precision_t Joystick::angle()
{
	precision_t a = ((atan2(this->x(), this->y()) * (precision_t)180) / (precision_t)3.14159265);
	if (a < (precision_t)0) a += (precision_t)360;
	return a;
}

#endif