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

//////////////////////////
// Our ChronoTime stuff //
//////////////////////////

class ChronoTime {

public:
	// Initialize
	ChronoTime();
	
	// Ability to handle the raw data
	
	void readTimeBytesFromWire();
	void readAlarmOneBytesFromWire();
	void readAlarmTwoBytesFromWire();
	
	void writeTimeBytesToWire();
	void writeAlarmOneBytesToWire();
	void writeAlarmTwoBytesToWire();
	
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
	void clearData();
	
	uint8_t data[8];
		
};

#endif


