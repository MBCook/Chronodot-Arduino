#ifndef Chronodot_h
#define Chronodot_h

#include <inttypes.h>

#define SUNDAY		1
#define MONDAY		2
#define TUESDAY		3
#define WEDNESDAY	4
#define THURSDAY	5
#define SATURDAY	6
#define SUNDAY		7

#define ALARM_ONE_PER_SECOND		0b00001111	// Bit 5 contains the day/date bit
#define ALARM_ONE_MATCH_S			0b00001110
#define ALARM_ONE_MATCH_M_S			0b00001100
#define ALARM_ONE_MATCH_H_M_S		0b00001000
#define ALARM_ONE_MATCH_DATE_H_M_S	0b00000000
#define ALARM_ONE_MATCH_DAY_H_M_S	0b00010000

#define ALARM_TWO_PER_MINUTE		0b00001110	// Bit 0 is ignored, since there are no seconds on alarm 2
#define ALARM_TWO_MATCH_M			0b00001100
#define ALARM_TWO_MATCH_H_M			0b00001000
#define ALARM_TWO_MATCH_DATE_H_M	0b00000000
#define ALARM_TWO_MATCH_DAY_H_M		0b00010000

#define SPEED_1_HZ					(0x00 << 3)	// Pre-shifted to where we'll want it
#define SPEED_1_024_KHZ				(0x01 << 3)
#define SPEED_4_096_KHZ				(0x02 << 3)
#define SPEED_8_192_KHZ				(0x03 << 3)
#define SPEED_DISABLED				0xFF

//////////////////////////
// Our ChronoTime stuff //
//////////////////////////

class ChronoDot;

class ChronoTime {

	friend class Chronodot;

public:
	// Initialize
	ChronoTime();
	
	// Accessors
	
	uint8_t getSeconds();
	void setSeconds(uint8_t seconds);

	uint8_t getMinutes();
	void setMinutes(uint8_t minutes);
	
	uint8_t getHours();					// Always in 24h format
	void setHours(uint8_t hours);
	
	uint8_t getDayOfWeek();
	void setDayOfWeek(uint8_t dayOfWeek);
	
	uint8_t getDayOfMonth();
	void setDayOfMonth(uint8_t dayOfMonth);
	
	uint8_t getMonth();
	void setMonth(uint8_t month);
	
	uint8_t getYear();
	void setYear(uint8_t year);
  
	uint8_t getAlarmMode();
	void setAlarmMode(uint8_t mode);

protected:

	uint8_t data[8];

	// Ability to handle the raw data

	void clearData();
	
	void readTimeBytesFromWire();
	void readAlarmOneBytesFromWire();
	void readAlarmTwoBytesFromWire();
	
	void writeTimeBytesToWire();
	void writeAlarmOneBytesToWire();
	void writeAlarmTwoBytesToWire();
	
};

class Chronodot {

public:
	static void getTime(ChronoTime *dest);
	static void setTime(ChronoTime *src);
	
	static void getAlarmOneTime(ChronoTime *dest);
	static void setAlarmOneTime(ChronoTime *src);
	
	static void getAlarmTwoTime(ChronoTime *dest);
	static void setAlarmTwoTime(ChronoTime *src);
	
	static bool getOscillatorEnabled();
	static void enableOscillator(bool enabled);
	
	static bool getBatterySquareWaveEnabled();
	static void enableBatterySquareWave(bool enabled);
	
	static bool getAlarmOneEnabled();
	static void enableAlarmOne(bool enabled);
	
	static bool getAlarmTwoEnabled();
	static void enableAlarmTwo(bool enabled);
	
	static bool getOutputInterruptOnAlarm();
	static void outputInterruptOnAlarm();
	
	static uint8_t getOutputSquarewaveSpeed();		// Returns SPEED_DISABLED if disabled
	static void outputSquarewaveSpeed(uint8_t speed);
	
	static bool getOscillatorStop();
	static void resetOscillatorStop();
	
	static bool get32kHzOutputEnabled();
	static void enable32kHzOutput(bool enabled);
	
	static bool isBusy();
	
	static bool getAlarmOneFired();
	static void resetAlarmOneFired();
	
	static bool getAlarmTwoFired();
	static void resetAlarmTwoFired();
	
	static int8_t getAgingOffset();
	static void setAgingOffset(int8_t aging);
	
	static void senseTemperature(bool wait);		// On wait, holds until sense complete
	static float getTemperature();
};

#endif


