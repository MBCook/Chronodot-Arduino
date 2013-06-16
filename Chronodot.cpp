#include <Arduino.h>
#include <Wire.h>

#include "Chronodot.h"

// The I2C address the Chronodot responds on

#define CHRONODOT_ADDRESS	0b01101000

// The registers on the Chronodot

#define REGISTER_TIME		0
#define REGISTER_ALARM_ONE	7
#define REGISTER_ALARM_TWO	11
#define REGISTER_CONTROL	14
#define REGISTER_STATUS		15
#define REGISTER_AGING		16
#define REGISTER_TEMP		17

// Some macros to make the code look cleaner

#define FROM_BCD_DIGIT(a)	(a)
#define FROM_BCD_DIGITS(a)	((((a) >> 4) * 10) + ((a) & 0x0F))

#define TO_BCD_DIGIT(a)		((a) % 10)
#define TO_BCD_DIGITS(a)	((((a) / 10) << 4) + TO_BCD_DIGIT(a))

#define BIT(a) (0x01 << (a))
#define MASK(a) (0xFF ^ BIT(a))

// The positions in the data structure the pieces of data are stored in.
// This mostly follows the Choronodot registers, with one extra for alarm metadata

#define SECOND_POSITION			0
#define MINUTE_POSITION			1
#define HOUR_POSITION			2
#define DAY_OF_WEEK_POSITION	3
#define DAY_OF_MONTH_POSITION	4
#define MONTH_POSITION			5
#define YEAR_POSITION			6
#define ALARM_POSITION			7

// Used to convert the high bits used to store alarm info into a byte we use for users

#define ALARM_BIT_ONE			0x01
#define ALARM_BIT_TWO			0x02
#define ALARM_BIT_THREE			0x04
#define ALARM_BIT_FOUR			0x08
#define ALARM_DAY_DATE			0x10

// The position of various bits in the registers that we care about

#define CENTURY_BIT				7	// Used in the time registers
#define ALARM_CONFIG_BIT		7
#define DAY_DATE_BIT			6

#define ENABLE_OSCILLATOR_BIT	7	// Control register
#define BATTERY_SQUARE_WAVE_BIT	6
#define TEMPERATURE_CONVERT_BIT	5
#define RATE_SELECT_BIT_TWO		4
#define RATE_SELECT_BIT_ONE		3
#define INTERUPT_BIT			2
#define ALARM_TWO_ENABLED_BIT	1
#define ALARM_ONE_ENABLED_BIT	0

#define OSCILLATOR_STOP_BIT		7	// Status register
#define THIRTY_TWO_KHZ_BIT		3
#define BUSY_BIT				2
#define ALARM_TWO_FLAG_BIT		1
#define ALARM_ONE_FLAG_BIT		0

/////////////////////
// Utility methods //
/////////////////////

void writeData(uint8_t registerAddress, uint8_t *data, uint8_t length) {
	// Start queueing data to the Chronodot, send the address of the register
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(registerAddress);
	
	// Send the data
	
	for (uint8_t i = 0; i < length; i++) {
		Wire.write(data[i]);
	}
	
	// Finish up
	
	Wire.endTransmission();
}

void readData(uint8_t registerAddress, uint8_t *dest, uint8_t length) {
	// Write the register address we want to start reading at
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(registerAddress);
	Wire.endTransmission();
	
	// Request the data
	
	Wire.requestFrom((uint8_t) CHRONODOT_ADDRESS, length);
	
	// Read back each byte
	
	for (uint8_t i = 0; i < length; i++) {
		// Wait for byte
		while (Wire.available() == 0) {}
		// Read it
		dest[i] = Wire.read();
	}
}

bool readBit(uint8_t registerAddress, uint8_t bit) {
	uint8_t temp;
	
	readData(registerAddress, &temp, 1);
	
	return temp & BIT(bit);
}

void updateRegister(uint8_t registerAddress, uint8_t mask, uint8_t data) {
	uint8_t temp;
	
	// Fetch the register, mask out the bits we're changing, set them to their new values...
	
	readData(registerAddress, &temp, 1);
	
	temp &= mask;
	temp |= data;
	
	// ... and write it back to the Chronodot
	
	writeData(registerAddress, &temp, 1);	
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

bool Chronodot::getOscillatorEnabled() {
	return readBit(REGISTER_CONTROL, ENABLE_OSCILLATOR_BIT);
}

void Chronodot::enableOscillator(bool enabled) {
	updateRegister(REGISTER_CONTROL, MASK(ENABLE_OSCILLATOR_BIT),
					enabled ? BIT(ENABLE_OSCILLATOR_BIT) : 0x00);
}

bool Chronodot::getBatterySquareWaveEnabled() {
	return readBit(REGISTER_CONTROL, BATTERY_SQUARE_WAVE_BIT);
}

void Chronodot::enableBatterySquareWave(bool enabled) {
	updateRegister(REGISTER_CONTROL, MASK(BATTERY_SQUARE_WAVE_BIT),
					enabled ? BIT(BATTERY_SQUARE_WAVE_BIT) : 0x00);
}

bool Chronodot::getAlarmOneEnabled() {
	return readBit(REGISTER_CONTROL, ALARM_ONE_ENABLED_BIT);
}

void Chronodot::enableAlarmOne(bool enabled) {
	updateRegister(REGISTER_CONTROL, MASK(ALARM_ONE_ENABLED_BIT),
					enabled ? BIT(ALARM_ONE_ENABLED_BIT) : 0x00);
}

bool Chronodot::getAlarmTwoEnabled() {
	return readBit(REGISTER_CONTROL, ALARM_TWO_ENABLED_BIT);
}

void Chronodot::enableAlarmTwo(bool enabled) {
	updateRegister(REGISTER_CONTROL, MASK(ALARM_TWO_ENABLED_BIT),
					enabled ? BIT(ALARM_TWO_ENABLED_BIT) : 0x00);
}

bool Chronodot::getOutputInterruptOnAlarm() {
	return readBit(REGISTER_CONTROL, INTERUPT_BIT);
}

void Chronodot::outputInterruptOnAlarm() {
	updateRegister(REGISTER_CONTROL, MASK(INTERUPT_BIT), BIT(INTERUPT_BIT));
}

uint8_t Chronodot::getOutputSquarewaveSpeed() {
	uint8_t temp;
	
	readData(REGISTER_CONTROL, &temp, 1);
	
	if (temp & MASK(INTERUPT_BIT) ^ BIT(INTERUPT_BIT)) {
		// We're in output interrupt mode, return invalid speed
		
		return SPEED_DISABLED;
	} else {
		// Speed mode. Get only the bits we want
		return temp & (MASK(RATE_SELECT_BIT_TWO) | MASK(RATE_SELECT_BIT_ONE));
	}
}

void Chronodot::outputSquarewaveSpeed(uint8_t speed) {
	if (speed > SPEED_8_192_KHZ)		// On invalid parameter, we'll do nothing	
		return;
		
	// We have three bits to mask out here, so this looks a little worse
	// We'll also bit-shift the speed setting into the appropriate position
		
	updateRegister(REGISTER_CONTROL, MASK(INTERUPT_BIT) & MASK(RATE_SELECT_BIT_TWO) & MASK(RATE_SELECT_BIT_ONE)
									, speed);
}

bool Chronodot::getOscillatorStop() {
	return readBit(REGISTER_STATUS, OSCILLATOR_STOP_BIT);
}

void Chronodot::resetOscillatorStop() {
	updateRegister(REGISTER_STATUS, MASK(OSCILLATOR_STOP_BIT), 0);
}

bool Chronodot::get32kHzOutputEnabled() {
	return readBit(REGISTER_STATUS, THIRTY_TWO_KHZ_BIT);
}

void Chronodot::enable32kHzOutput(bool enabled) {
	updateRegister(REGISTER_STATUS, MASK(REGISTER_STATUS),
					enabled ? BIT(REGISTER_STATUS) : 0x00);
}

bool Chronodot::isBusy() {
	return readBit(REGISTER_STATUS, BUSY_BIT);
}

bool Chronodot::getAlarmOneFired() {
	return readBit(REGISTER_STATUS, ALARM_ONE_FLAG_BIT);
}

void Chronodot::resetAlarmOneFired() {
	updateRegister(REGISTER_STATUS, MASK(ALARM_ONE_FLAG_BIT), 0);
}

bool Chronodot::getAlarmTwoFired() {
	return readBit(REGISTER_STATUS, ALARM_TWO_FLAG_BIT);
}

void Chronodot::resetAlarmTwoFired() {
	updateRegister(REGISTER_STATUS, MASK(ALARM_TWO_FLAG_BIT), 0);
}

int8_t Chronodot::getAgingOffset() {
	uint8_t temp;
	
	readData(REGISTER_AGING, &temp, 1);
	
	return temp;
}

void Chronodot::setAgingOffset(int8_t aging) {
	writeData(REGISTER_AGING, (uint8_t *) &aging, 1);
}

void Chronodot::senseTemperature(bool wait) {
	updateRegister(REGISTER_CONTROL, MASK(TEMPERATURE_CONVERT_BIT), BIT(TEMPERATURE_CONVERT_BIT));
	
	// Wait until the busy flag is clear
	
	while(wait && Chronodot::isBusy()) {};
}

float Chronodot::getTemperature() {
	uint8_t data[2] = {0, 0};
	
	readData(REGISTER_TEMP, (uint8_t *) &data, 2);
	
	uint8_t whole = data[0];
	uint8_t frac = data[1];
	
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
	
	readData(REGISTER_TIME, this->data, 7);
}

void ChronoTime::readAlarmOneBytesFromWire() {
	this->clearData();
	
	// Get the four bytes of alarm one's data
	
	readData(REGISTER_ALARM_ONE, this->data, 4);
	
	// Figure out if it's day or date
	
	bool day = this->data[DAY_OF_WEEK_POSITION] & MASK(DAY_DATE_BIT);
	
	if (!day) {
		// If it's date mode, we need to move the byte, but keep the day/date bit in place
		
		this->data[DAY_OF_MONTH_POSITION] = this->data[DAY_OF_WEEK_POSITION];
		this->data[DAY_OF_WEEK_POSITION] = this->data[DAY_OF_WEEK_POSITION] & BIT(DAY_DATE_BIT);
	}
		
	// Now we'll synthesize the alarm type byte
	
	this->data[ALARM_POSITION] = (this->data[SECOND_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_ONE)
									| (this->data[MINUTE_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_TWO)
									| (this->data[HOUR_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_THREE)
									| (this->data[DAY_OF_WEEK_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_FOUR)
									| (day ? BIT(DAY_DATE_BIT) : 0x00);
}

void ChronoTime::readAlarmTwoBytesFromWire() {
	this->clearData();
	
	// Get the three bytes of alarm two's data
	
	readData(REGISTER_ALARM_TWO, this->data, 3);
		
	// Figure out if it's day or date
	
	bool day = this->data[DAY_OF_WEEK_POSITION] & MASK(DAY_DATE_BIT);
	
	if (!day) {
		// If it's date mode, we need to move the byte, but keep the day/date bit in place
		this->data[DAY_OF_MONTH_POSITION] = this->data[DAY_OF_WEEK_POSITION];
		this->data[DAY_OF_WEEK_POSITION] = this->data[DAY_OF_WEEK_POSITION] & DAY_DATE_BIT;
	}
		
	// Now we'll synthesize the alarm type byte
	
	this->data[ALARM_POSITION] = (this->data[MINUTE_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_TWO)
									| (this->data[HOUR_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_THREE)
									| (this->data[DAY_OF_WEEK_POSITION] & BIT(ALARM_CONFIG_BIT)) >> (8 - ALARM_BIT_FOUR)
									| (day ? BIT(DAY_DATE_BIT) : 0x00);
}

void ChronoTime::writeTimeBytesToWire() {
	writeData(REGISTER_TIME, this->data, 7);
}

void ChronoTime::writeAlarmOneBytesToWire() {
	// This is complicated enough we'll do it by hand so we don't need to make an extra array
	
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);

	Wire.write(REGISTER_ALARM_ONE);

	// First 3 bytes are normal, and we use the high bit to transfer part of the alarm type

	Wire.write(this->data[SECOND_POSITION] | (alarmType & ALARM_BIT_ONE ? BIT(ALARM_CONFIG_BIT) : 0));
	Wire.write(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? BIT(ALARM_CONFIG_BIT) : 0));
	Wire.write(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? BIT(ALARM_CONFIG_BIT) : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		Wire.write(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? BIT(ALARM_CONFIG_BIT) : 0) | BIT(DAY_DATE_BIT));
	} else {
		Wire.write(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? BIT(ALARM_CONFIG_BIT) : 0));
	}
		
	Wire.endTransmission();
}

void ChronoTime::writeAlarmTwoBytesToWire() {
	// This is complicated enough we'll do it by hand so we don't need to make an extra array
	
	uint8_t alarmType = this->data[ALARM_POSITION];
	
	Wire.beginTransmission(CHRONODOT_ADDRESS);
	Wire.write(REGISTER_ALARM_TWO);

	// First two bytes are normal, and we use the high bit to transfer part of the alarm type

	Wire.write(this->data[MINUTE_POSITION] | (alarmType & ALARM_BIT_TWO ? BIT(ALARM_CONFIG_BIT) : 0));
	Wire.write(this->data[HOUR_POSITION] | (alarmType & ALARM_BIT_THREE ? BIT(ALARM_CONFIG_BIT) : 0));
	
	// Now we need to send the right byte, either the day or the date
	
	if (alarmType & ALARM_DAY_DATE) {	
		Wire.write(this->data[DAY_OF_WEEK_POSITION] | (alarmType & ALARM_BIT_FOUR ? BIT(ALARM_CONFIG_BIT) : 0) | BIT(DAY_DATE_BIT));
	} else {
		Wire.write(this->data[DAY_OF_MONTH_POSITION] | (alarmType & ALARM_BIT_FOUR ? BIT(ALARM_CONFIG_BIT) : 0));
	}
		
	Wire.endTransmission();
}

void ChronoTime::setAlarmMode(uint8_t mode) {
	this->data[ALARM_POSITION] = mode;
}

uint8_t ChronoTime::getAlarmMode() {
	return this->data[ALARM_POSITION];
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
	
	this->data[HOUR_POSITION] = 0x3f & TO_BCD_DIGITS(hours % 24);
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
	this->data[DAY_OF_MONTH_POSITION] = ((dayOfMonth - 1) % 31) + 1;
}

uint8_t ChronoTime::getMonth() {
	// Special handling because of the century,
	// so we have to mask off the first bit of the byte
	
	return FROM_BCD_DIGITS(0x7F & this->data[MONTH_POSITION]);
}

void ChronoTime::setMonth(uint8_t month) {
	// Special handling to keep the century bit
	
	this->data[MONTH_POSITION] = (this->data[MONTH_POSITION] & 0x80) + TO_BCD_DIGITS(((month - 1) % 12) + 1);
}

uint8_t ChronoTime::getYear() {
	// Special handling to get the century bit from the month position
	
	return (this->data[MONTH_POSITION] & 0x80 ? 100 : 0)
			+ FROM_BCD_DIGITS(this->data[YEAR_POSITION]);
}

void ChronoTime::setYear(uint8_t year) {
	this->data[MONTH_POSITION] = (this->data[MONTH_POSITION] & 0x7F) + (year >= 100 ? 0x80 : 0x00);
	this->data[YEAR_POSITION] = TO_BCD_DIGITS(year % 100);
}