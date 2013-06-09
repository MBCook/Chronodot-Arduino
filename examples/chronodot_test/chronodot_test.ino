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

			Serial.println(lastInput);

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

	bool batteryOscillator = Chronodot::getOscillatorEnabled();
	
	Serial.print("Oscillator enabled on battery is ");
	Serial.println(batteryOscillator);

	bool batterySquarewave = Chronodot::getBatterySquareWaveEnabled();
	
	Serial.print("Battery backed square wave enabled is ");
	Serial.println(batterySquarewave);
	
	uint8_t speed = Chronodot::getOutputSquarewaveSpeed();
	
	switch (speed) {
		case SPEED_1_HZ:
			Serial.println("Square wave output is 1 Hz");
			break;
		case SPEED_1_024_KHZ:
			Serial.println("Square wave output is 1.024 kHz");
			break;
		case SPEED_4_096_KHZ:
			Serial.println("Square wave output is 4.096 kHz");
			break;
		case SPEED_8_192_KHZ:
			Serial.println("Square wave output is 8.192 kHz");
			break;
		default:
			Serial.println("Square wave off, interupt on alarm");
	}
	
	bool thirtyTwo = Chronodot::get32kHzOutputEnabled();
	
	Serial.print("32kHz otuput enabled is ");
	Serial.println(thirtyTwo);
	
	bool oscillatorStop = Chronodot::getOscillatorStop();
	
	Serial.print("Oscillator was stopped is ");
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
	while (true) {
		Serial.println("");
		Serial.println("General Settings");
		Serial.println("---------");
		Serial.println("1. Toggle battery oscillator");
		Serial.println("2. Toggle battery square wave");	
		Serial.println("3. Trigger temperature reading");
		Serial.println("4. Set interrupt on alarm");
		Serial.println("5. Output 1 Hz square wave");
		Serial.println("6. Output 1.024 kHz square wave");
		Serial.println("7. Output 4.096 kHz square wave");
		Serial.println("8. Output 8.196 kHz square wave");
		Serial.println("9. Reset oscillator stop");
		Serial.println("0. Toggle 32 kHz out");
		Serial.println("");
		Serial.println("x. Back to main menu");
		Serial.println("");
	
		lastInput = 0;
	
		while (lastInput == 0) {
			Serial.print("? ");
		
			while (Serial.available() == 0) {
				// Just wait
			}
		
			lastInput = Serial.read();

			Serial.println(lastInput);

			while (Serial.available() > 0)	// Eat up any other input
				Serial.read();
			
			Serial.println("");
			
			bool old;		// Not allowed in the switch with this compiler
			
			switch (lastInput) {
				case '1':
					old = Chronodot::getOscillatorEnabled();
					
					Chronodot::enableOscillator(!old);
					
					Serial.print("Battery oscillator is now ");
					Serial.println(!old);
					
					break;
				case '2':
					old = Chronodot::getBatterySquareWaveEnabled();
					
					Chronodot::enableBatterySquareWave(!old);
					
					Serial.print("Battery square wave is now ");
					Serial.println(!old);
					
					break;
				case '3':
					Chronodot::senseTemperature(true);
					
					Serial.println("Temperature reading started");

					Serial.print("Temperature is ");
					Serial.print(Chronodot::getTemperature());
					Serial.println(" C");
					
					break;
				case '4':
					Chronodot::outputInterruptOnAlarm();
					
					Serial.println("Interrupt on alarm set");

					break;
				case '5':
					Chronodot::outputSquarewaveSpeed(SPEED_1_HZ);
					
					Serial.println("Squarewave output is 1 Hz");

					break;
				case '6':
					Chronodot::outputSquarewaveSpeed(SPEED_1_024_KHZ);
					
					Serial.println("Squarewave output is 1.024 kHz");

					break;				
				case '7':
					Chronodot::outputSquarewaveSpeed(SPEED_4_096_KHZ);
					
					Serial.println("Squarewave output is 4.096 kHz");

					break;				
				case '8':
					Chronodot::outputSquarewaveSpeed(SPEED_8_192_KHZ);
					
					Serial.println("Squarewave output is 8.192 kHz");

					break;				
				case '9':
					Chronodot::resetOscillatorStop();
					
					Serial.println("Oscillator stop flag reset");

					break;
				case '0':
					old = Chronodot::get32kHzOutputEnabled();
					
					Chronodot::enable32kHzOutput(!old);
					
					Serial.print("32kHz output enabled is ");
					Serial.println(!old);
					
					break;
				case 'x':
				case 'X':
					return;					// Back to the main menu
				default:
					lastInput = 0;			// Bad input, force the prompt to re-display
			}
		}
	}
}
