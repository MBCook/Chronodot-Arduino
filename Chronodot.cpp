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
#define FROM_BCD_DIGITS(a)	((((a) >> 4) * 10) + ((a) & 0x0F))
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

/////////////////////
// Utility methods //
/////////////////////

void startWrite(uint8_t registerAddress) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(registerAddress);
}

void writeByte(uint8_t byte) {
	Wire.write(byte);
}

void writeBytes(uint8_t *bytes, uint8_t count) {
	Wire.write(bytes, count);
}

void endWrite() {
	Wire.endTransmission();
}
	
void startRead(uint8_t registerAddress, uint8_t bytes) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(registerAddress);
	Wire.endTransmission();
	Wire.requestFrom((uint8_t) CHRONODOT_ADDRESS, bytes);
}

uint8_t readByte() {
	while (Wire.available() == 0) {
		// Wait
	}
	
	return Wire.read();
}

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
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = (control & 0x7F) | (enabled ? 0x80 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();	
}

void Chronodot::updateBatterySquareWave(bool enabled) {
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = (control & 0xBF) | (enabled ? 0x40 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();	
}

void Chronodot::updateAlarmOne(bool enabled) {
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = (control & 0xFE) | (enabled ? 0x01 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();
}

void Chronodot::updateAlarmTwo(bool enabled) {
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = (control & 0xFD) | (enabled ? 0x02 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();
}

void Chronodot::outputInterruptOnAlarm() {
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = control | 0xFB;	// Set the bit, that means issue interrupt on alarm match
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();
}

void Chronodot::outputSquarewave(uint8_t speed) {
	startRead(REGISTER_CONTROL, 1);
	
	uint8_t control = readByte();
	
	control = control & 0xE3;	// Clear the bit, that means output a squarewave, clear the speed bits
	
	control = control | ((speed & 0x03) << 3);	// Set the speed bits
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_CONTROL);
	writeByte(control);
	endWrite();
}

bool Chronodot::getOscilatorStop() {
	startRead(REGISTER_STATUS, 1);
	
	return readByte() & 0x80;
}

void Chronodot::resetOscilatorStop() {
	startRead(REGISTER_STATUS, 1);
	
	uint8_t status = readByte();
	
	status = status & 0x7F;		// Clear the oscillator stop bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_STATUS);
	writeByte(status);
	endWrite();
}

void Chronodot::update32kHzOutput(bool enabled) {
	startRead(REGISTER_STATUS, 1);
	
	uint8_t status = readByte();
	
	status = (status & 0xF7) | (enabled ? 0x08 : 0x00);
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_STATUS);
	writeByte(status);
	endWrite();	
}

bool Chronodot::isBusy() {
	startRead(REGISTER_STATUS, 1);
	
	return readByte() & 0x04;
}

bool Chronodot::alarmOneFired() {
	startRead(REGISTER_STATUS, 1);
	
	return readByte() & 0x01;
}

void Chronodot::resetAlarmOne() {
	startRead(REGISTER_STATUS, 1);
	
	uint8_t status = readByte();
	
	status = status & 0xFE;	// Clear the bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_STATUS);
	writeByte(status);
	endWrite();	
}

bool Chronodot::alarmTwoFired() {
	startRead(REGISTER_STATUS, 1);
	
	return readByte() & 0x02;
}

void Chronodot::resetAlarmTwo() {
	startRead(REGISTER_STATUS, 1);
	
	uint8_t status = readByte();
	
	status = status & 0xFD;	// Clear the bit
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_STATUS);
	writeByte(status);
	endWrite();	
}

int8_t Chronodot::getAgingOffset() {
	startRead(REGISTER_AGING, 1);
	
	return (int8_t) readByte();
}

void Chronodot::setAgingOffset(int8_t aging) {
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	writeByte(REGISTER_AGING);
	writeByte((uint8_t) aging);
	endWrite();	
}

float Chronodot::getTemperature() {
	startRead(REGISTER_TEMP, 2);
	
	uint8_t whole = readByte();
	uint8_t frac = readByte();
	
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
	
	startRead(REGISTER_TIME, 7);

	for (uint8_t i = 0; i < 7; i++) {
		this->data[i] = readByte();
	}
}

void ChronoTime::readAlarmOneBytesFromWire() {
	this->clearData();
	
	startRead(REGISTER_ALARM_ONE, 4);
	
	for (uint8_t i = 0; i < 4; i++)
		this->data[i] = readByte();
		
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
	
	startRead(REGISTER_ALARM_TWO, 3);
		
	for (uint8_t i = 0; i < 3; i++)
		this->data[i + 1] = readByte();
		
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
	writeByte(REGISTER_TIME);
	writeBytes(this->data, 7);
	endWrite();
}

void ChronoTime::writeAlarmOneBytesToWire() {
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);

	writeByte(REGISTER_ALARM_ONE);

	// First 3 bytes are normal, and we use the high bit to transfer part of the alarm type

	writeByte(this->data[SECOND_POSITION] | (alarmType & ALARM_BIT_ONE ? 0x80 : 0));
	writeByte(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? 0x80 : 0));
	writeByte(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? 0x80 : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		writeByte(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0) | 0x40);
	} else {
		writeByte(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0));
	}
		
	endWrite();
}

void ChronoTime::writeAlarmTwoBytesToWire() {
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);

	writeByte(REGISTER_ALARM_ONE);

	// First two bytes are normal, and we use the high bit to transfer part of the alarm type

	writeByte(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? 0x80 : 0));
	writeByte(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? 0x80 : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		writeByte(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0) | 0x40);
	} else {
		writeByte(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? 0x80 : 0));
	}
		
	endWrite();
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