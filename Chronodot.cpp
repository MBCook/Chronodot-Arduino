#include <Arduino.h>
#include <Wire.h>

#include "Chronodot.h"

#define CHRONODOT_ADDRESS	0b01101000

#define REGISTER_TIME		0
#define REGISTER_ALARM_ONE	7
#define REGISTER_ALARM_TWO	11
#define REGISTER_CONTROL	14
#define REGISTER_STATUS		15
#define REGISTER_AGING		16
#define REGISTER_TEMP_WHOLE	17
#define REGISTER_TEMP_FRAC	18

#define FROM_BCD_DIGIT(a)	a
#define FROM_BCD_DIGITS(a)	((((a) >> 4) * 10) + (a))
#define TO_BCD_DIGIT(a)		((a) % 10)
#define TO_BCD_DIGITS(a)	((((a) / 10) << 4) + TO_BCD_DIGIT(a))

#define SECOND_POSITION			0
#define MINUTE_POSITION			1
#define HOUR_POSITION			2
#define DAY_OF_WEEK_POSITION	3
#define DAY_OF_MONTH_POSITION	4
#define MONTH_POSITION			5
#define YEAR_POSITION			6
#define ALARM_POSITION			7

#define ALARM_BIT_ONE			0x01
#define ALARM_BIT_TWO			0x02
#define ALARM_BIT_THREE			0x04
#define ALARM_BIT_FOUR			0x08
#define ALARM_DAY_DATE			0x10

//////////////////////////
// Our ChronoTime stuff //
//////////////////////////

ChronoTime::ChronoTime() {
	this->clearData();
}

void ChronoTime::clearData() {
	memset(this->data, 0, sizeof(this->data));
}
	
void ChronoTime::readTimeBytesFromWire() {
	this->clearData();
	
	// This is the easy case, just read all the bytes in
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_TIME);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 7);
	
	for (uint8_t i = 0; i < 7; i++)
		this->data[i] = Wire.read();
}

void ChronoTime::readAlarmOneBytesFromWire() {
	this->clearData();
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_ALARM_ONE);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 4);
	
	for (uint8_t i = 0; i < 4; i++)
		this->data[i] = Wire.read();
		
	// Figure out if it's day or date
	
	uint8_t day = (this->data[DAY_OF_WEEK_POSITION] & 0x40) > 0 ? 1 : 0;
	
	if (!day) {
		// If it's date mode, we need to move the byte, but keep the day/date bit in place
		this->data[DAY_OF_MONTH_POSITION] = this->data[DAY_OF_WEEK_POSITION];
		this->data[DAY_OF_WEEK_POSITION] = this->data[DAY_OF_WEEK_POSITION] & 0x40;
	}
		
	// Now we'll synthesize the alarm type byte
	
	this->data[ALARM_POSITION] = (this->data[SECOND_POSITION] & 0x80) >> (8 - ALARM_BIT_ONE)
									| (this->data[MINUTE_POSITION] & 0x80) >> (8 - ALARM_BIT_TWO)
									| (this->data[HOUR_POSITION] & 0x80) >> (8 - ALARM_BIT_THREE)
									| (this->data[DAY_OF_WEEK_POSITION] & 0x80) >> (8 - ALARM_BIT_FOUR)
									| day << 4;
}

void ChronoTime::readAlarmTwoBytesFromWire() {
	this->clearData();
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_ALARM_TWO);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 3);
	
	for (uint8_t i = 0; i < 3; i++)
		this->data[i + 1] = Wire.read();
		
	// Figure out if it's day or date
	
	uint8_t day = (this->data[DAY_OF_WEEK_POSITION] & 0x40) > 0 ? 1 : 0;
	
	if (!day) {
		// If it's date mode, we need to move the byte, but keep the day/date bit in place
		this->data[DAY_OF_MONTH_POSITION] = this->data[DAY_OF_WEEK_POSITION];
		this->data[DAY_OF_WEEK_POSITION] = this->data[DAY_OF_WEEK_POSITION] & 0x40;
	}
		
	// Now we'll synthesize the alarm type byte
	
	this->data[ALARM_POSITION] = (this->data[MINUTE_POSITION] & 0x80) >> (8 - ALARM_BIT_TWO)
									| (this->data[HOUR_POSITION] & 0x80) >> (8 - ALARM_BIT_THREE)
									| (this->data[DAY_OF_WEEK_POSITION] & 0x80) >> (8 - ALARM_BIT_FOUR)
									| day << 4;
}

void ChronoTime::writeTimeBytesToWire() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_TIME);
	Wire.write(this->data, 7);
	Wire.endTransmission();
}

void ChronoTime::writeAlarmOneBytesToWire() {
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);

	Wire.write(REGISTER_ALARM_ONE);

	// First 3 bytes are normal, and we use the high bit to transfer part of the alarm type

	Wire.write(this->data[SECOND_POSITION] | (alarmType & ALARM_BIT_ONE ? 0x80 : 0));
	Wire.write(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? 0x80 : 0));
	Wire.write(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? 0x80 : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		Wire.write(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0) | 0x40);
	} else {
		Wire.write(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0));
	}
		
	Wire.endTransmission();
}

void ChronoTime::writeAlarmTwoBytesToWire() {
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);

	Wire.write(REGISTER_ALARM_ONE);

	// First two bytes are normal, and we use the high bit to transfer part of the alarm type

	Wire.write(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? 0x80 : 0));
	Wire.write(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? 0x80 : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		Wire.write(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0) | 0x40);
	} else {
		Wire.write(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0));
	}
		
	Wire.endTransmission();
}

uint8_t ChronoTime::getSeconds() {
	return FROM_BCD_DIGITS(this->data[SECOND_POSITION]);
}

void ChronoTime::setSeconds(uint8_t seconds) {
	this->data[SECOND_POSITION] = TO_BCD_DIGITS(seconds % 60);
}

uint8_t ChronoTime::getMinutes() {
	return FROM_BCD_DIGITS(this->data[MINUTE_POSITION]);
}

void ChronoTime::setMinutes(uint8_t minutes) {
	this->data[MINUTE_POSITION] = TO_BCD_DIGITS(minutes % 60);
}

uint8_t ChronoTime::getHours() {
	// Special handling here for 12/24 hour stuff
	
	return FROM_BCD_DIGITS(this->data[HOUR_POSITION]);
}

void ChronoTime::setHours(uint8_t hours) {
	// Special handling to ensure the Chronodot knows we're in 24 hour mode
	
	this->data[MINUTE_POSITION] = 0x3f & TO_BCD_DIGITS(hours % 24);
}

uint8_t ChronoTime::getDayOfWeek() {
	return this->data[DAY_OF_WEEK_POSITION];
}

void ChronoTime::setDayOfWeek(uint8_t dayOfWeek) {
	this->data[DAY_OF_WEEK_POSITION] = ((dayOfWeek - 1) % 7) + 1;
}

uint8_t ChronoTime::getDayOfMonth() {
	return FROM_BCD_DIGITS(this->data[DAY_OF_MONTH_POSITION]);
}

void ChronoTime::setDayOfMonth(uint8_t dayOfMonth) {
	this->data[DAY_OF_MONTH_POSITION] = dayOfMonth % 31;
}

uint8_t ChronoTime::getMonth() {
	// Special handling because of the century,
	// so we have to mask off the first bit of the byte
	
	return FROM_BCD_DIGITS(0x7F & this->data[HOUR_POSITION]);
}

void ChronoTime::setMonth(uint8_t month) {
	// Special handling to keep the century bit
	
	this->data[MONTH_POSITION] = this->data[MONTH_POSITION] & 0x80 + TO_BCD_DIGITS(month % 31);
}

uint8_t ChronoTime::getYear() {
	// Special handling to get the century bit from the month position
	
	return this->data[MONTH_POSITION] & 0x80 ? 100 : 0
			+ FROM_BCD_DIGITS(this->data[YEAR_POSITION]);
}

void ChronoTime::setYear(uint8_t year) {
	this->data[MONTH_POSITION] = this->data[MONTH_POSITION] & 0x7F + (year >= 100 ? 0x80 : 0x00);
	this->data[YEAR_POSITION] = TO_BCD_DIGITS(year % 100);
}