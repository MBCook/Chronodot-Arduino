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

#define SPEED_1_HZ					0x00
#define SPEED_1_024_KHZ				0x01
#define SPEED_4_096_KHZ				0x02
#define SPEED_8_192_KHZ				0x03

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
	
	static void updateOscilator(bool enabled);
	static void updateBatterySquareWave(bool enabled);
	static void updateAlarmOne(bool enabled);
	static void updateAlarmTwo(bool enabled);
	
	static void outputInterruptOnAlarm();
	static void outputSquarewave(uint8_t speed);
	
	static bool getOscilatorStop();
	static void resetOscilatorStop();
	
	static void update32kHzOutput(bool enabled);
	
	static bool isBusy();
	
	static bool alarmOneFired();
	static void resetAlarmOne();
	
	static bool alarmTwoFired();
	static void resetAlarmTwo();
	
	static int8_t getAgingOffset();
	static void setAgingOffset(int8_t aging);
	
	static float getTemperature();
};

#endif


