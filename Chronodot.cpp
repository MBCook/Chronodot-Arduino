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
#define REGISTER_TEMP		17

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

/////////////////////////
// Our Chronodot stuff //
/////////////////////////

void Chronodot::getTime(ChronoTime *dest) {
	dest->readTimeBytesFromWire();
}

void Chronodot::setTime(ChronoTime *src) {
	src->writeTimeBytesToWire();
}

void Chronodot::getAlarmOneTime(ChronoTime *dest) {
	dest->readAlarmOneBytesFromWire();
}

void Chronodot::setAlarmOneTime(ChronoTime *src) {
	src->writeAlarmOneBytesToWire();
}

void Chronodot::getAlarmTwoTime(ChronoTime *dest) {
	dest->readAlarmTwoBytesFromWire();
}

void Chronodot::setAlarmTwoTime(ChronoTime *src) {
	src->writeAlarmTwoBytesToWire();
}

void Chronodot::updateOscilator(bool enabled) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = (control & 0x7F) | (enabled ? 0x80 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();	
}

void Chronodot::updateBatterySquareWave(bool enabled) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = (control & 0xBF) | (enabled ? 0x40 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();	
}

void Chronodot::updateAlarmOne(bool enabled) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = (control & 0xFE) | (enabled ? 0x01 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();
}

void Chronodot::updateAlarmTwo(bool enabled) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = (control & 0xFD) | (enabled ? 0x02 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();
}

void Chronodot::outputInterruptOnAlarm() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = control | 0xFB;	// Set the bit, that means issue interrupt on alarm match
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();
}

void Chronodot::outputSquarewave(uint8_t speed) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t control = Wire.read();
	
	control = control & 0xE3;	// Clear the bit, that means output a squarewave, clear the speed bits
	
	control = control | ((speed & 0x03) << 3);	// Set the speed bits
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_CONTROL);
	Wire.write(control);
	Wire.endTransmission();
}

bool Chronodot::getOscilatorStop() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	return Wire.read() & 0x80;
}

void Chronodot::resetOscilatorStop() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t status = Wire.read();
	
	status = status & 0x7F;		// Clear the oscillator stop bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.write(status);
	Wire.endTransmission();
}

void Chronodot::update32kHzOutput(bool enabled) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t status = Wire.read();
	
	status = (status & 0xF7) | (enabled ? 0x08 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.write(status);
	Wire.endTransmission();	
}

bool Chronodot::isBusy() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	return Wire.read() & 0x04;
}

bool Chronodot::alarmOneFired() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	return Wire.read() & 0x01;
}

void Chronodot::resetAlarmOne() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t status = Wire.read();
	
	status = status & 0xFE;	// Clear the bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.write(status);
	Wire.endTransmission();	
}

bool Chronodot::alarmTwoFired() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	return Wire.read() & 0x02;
}

void Chronodot::resetAlarmTwo() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	uint8_t status = Wire.read();
	
	status = status & 0xFD;	// Clear the bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_STATUS);
	Wire.write(status);
	Wire.endTransmission();	
}

int8_t Chronodot::getAgingOffset() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_AGING);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 1);
	
	return (int8_t) Wire.read();
}

void Chronodot::setAgingOffset(int8_t aging) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_AGING);
	Wire.write((uint8_t) aging);
	Wire.endTransmission();	
}

float Chronodot::getTemperature() {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_TEMP);
	Wire.endTransmission();
	Wire.requestFrom(CHRONODOT_ADDRESS, 2);
	
	uint8_t whole = Wire.read();
	uint8_t frac = Wire.read();
	
	float temp = (float) whole;
	
	temp = temp + (frac & 0x80 ? 0.5 : 0.0);
	temp = temp + (frac & 0x40 ? 0.25 : 0.0);
	
	return temp;
}

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
	
	Serial.println("");
	
	for (uint8_t i = 0; i < 7; i++) {
		while (Wire.available() == 0) {
			// Wait for data
		}
		this->data[i] = Wire.read();
		Serial.print("Byte in: ");
		Serial.println(this->data[i]);
	}
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
	
	return FROM_BCD_DIGITS(0x7F & this->data[MONTH_POSITION]);
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