#include <Wire.h>
#include <Chronodot.h>
#include <Arduino.h>

#define MAIN_MENU			0
#define CHANGE_TIME			1
#define ALARM_MENU			2
#define CHANGE_ALARM_ONE	3
#define CHANGE_ALARM_TWO	4
#define SETTINGS			5

ChronoTime myTime;
char currentMenu;
char lastInput;

char *daysOfWeek[] = {"___", "Sunday", "Monday", "Tuesday", "Wednesday",
								"Thursday", "Friday", "Saturday"};
char *monthNames[] = {"___", "January", "February", "March", "April", "May",
								"June", "July", "August", "September",
								"October", "November", "December"};

void setup() {
	currentMenu = MAIN_MENU;
	
	Serial.begin(9600);

	Wire.begin();
}

void loop() {
	while (!Serial) {
		// Wait for the serial
	}
	
	while (true) {	
  
		Serial.println("");
		Serial.println("Main Menu");
		Serial.println("---------");
		Serial.println("1. Show time");
		Serial.println("2. Show status");	
		Serial.println("3. Change time");
		Serial.println("4. Alarm settings");
		Serial.println("5. General settings");
		Serial.println("");
		
		lastInput = 0;
		
		while (lastInput == 0) {
			Serial.print("? ");
			
			while (Serial.available() == 0) {
				// Just wait
			}
			
			lastInput = Serial.read();
			
                        Serial.print(lastInput);        // Echo it back out

			while (Serial.available() > 0)	// Eat up any other input
				Serial.read();
				
			Serial.println("");
				
			switch (lastInput) {
				case '1':
					showTime();
					break;
				case '2':
					showStatus();
					break;
				case '3':
					changeTime();
					break;
				case '4':
					alarmSettings();
					break;
				case '5':
					generalSettings();
					break;
				default:
					lastInput = 0;			// Bad input, force the prompt to re-display
			}
		}
	}
}

void showTime() {
	Serial.println("");
	
	// Read in the current time
	
	Chronodot::getTime(&myTime);	
	
	// Display it
	
	uint8_t dayOfWeek = myTime.getDayOfWeek();
	
	if (dayOfWeek > 7)
		dayOfWeek = 0;
		
	Serial.print(daysOfWeek[dayOfWeek]);
	
	Serial.print(" ");
	
	uint8_t month = myTime.getMonth();
	
	if (month > 12)
		month = 0;
		
	Serial.print(monthNames[month]);
	
	Serial.print(" ");
	
	Serial.print(myTime.getDayOfMonth());
	
	Serial.print(", ");
	
	Serial.print(2000 + myTime.getYear());
	
	Serial.print(" - ");
	
	Serial.print(myTime.getHours());
	
	Serial.print(":");
	
	if (myTime.getMinutes() < 10)
		Serial.print("0");
	
	Serial.print(myTime.getMinutes());
	
	Serial.print(":");
	
	if (myTime.getSeconds() < 10)
		Serial.print("0");
		
	Serial.println(myTime.getSeconds());
}

void showStatus() {
	Serial.println("");
	
	bool oscillatorStop = Chronodot::getOscillatorStop();
	
	Serial.print("Oscillator stop is ");
	Serial.println(oscillatorStop);
	
	bool busy = Chronodot::isBusy();
	
	Serial.print("Busy flag is ");
	Serial.println(busy);
	
	bool alarm = Chronodot::getAlarmOneFired();
	
	Serial.print("Alarm one fired is ");
	Serial.println(alarm);
	
	alarm = Chronodot::getAlarmTwoFired();
	
	Serial.print("Alarm two fired is ");
	Serial.println(alarm);
	
	int8_t agingOffset = Chronodot::getAgingOffset();
	
	Serial.print("Aging offset is ");
	Serial.println(agingOffset);
	
	float temp = Chronodot::getTemperature();
	
	Serial.print("Temperature is ");
	Serial.print(temp);
	Serial.println(" C");
}

void changeTime() {

}

void alarmMenu() {

}

void alarmSettings() {

}

void generalSettings() {

}
