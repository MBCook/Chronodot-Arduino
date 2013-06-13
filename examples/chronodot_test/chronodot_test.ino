#include <Wire.h>
#include <Chronodot.h>
#include <Arduino.h>

ChronoTime myTime;		// The time variable we'll use
char lastInput;			// The last byte we read from serial
char buffer[100];		// For getting input

// Human readable names for the times we'll be printing out

char *daysOfWeek[] = {"___", "Sunday", "Monday", "Tuesday", "Wednesday",
								"Thursday", "Friday", "Saturday"};
char *monthNames[] = {"___", "January", "February", "March", "April", "May",
								"June", "July", "August", "September",
								"October", "November", "December"};

#define GOT_STRING 1	// Useful for one of our functions
#define EXIT_MENU 2
#define TOO_LONG 3

////////////////////
// Setup function //
////////////////////

void setup() {
	// Enable the serial port so the console works
	
	Serial.begin(9600);

	// Setup the I2C bus so we can actually talk to the Chronodot

	Wire.begin();
}

////////////////
// Main loop //
///////////////

void loop() {
	while (!Serial) {
		// Wait for the serial
	}
	
	while (true) {	
  
		Serial.println(F(""));
		Serial.println(F("Main Menu"));
		Serial.println(F("---------"));
		Serial.println(F("1. Show time"));
		Serial.println(F("2. Show status"));	
		Serial.println(F("3. Change time"));
		Serial.println(F("4. Alarm settings"));
		Serial.println(F("5. General settings"));
		Serial.println(F(""));
		
		lastInput = 0;
		
		while (lastInput == 0) {
			Serial.print(F("? "));
			
			while (Serial.available() == 0) {
				// Just wait
			}
			
			lastInput = Serial.read();

			Serial.println(lastInput);

			while (Serial.available() > 0)	// Eat up any other input
				Serial.read();
				
			Serial.println(F(""));
				
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

///////////////////////////////////
// Display the time for the user //
///////////////////////////////////

void showTime() {
	// Read in the current time
	
	Chronodot::getTime(&myTime);	
	
	// Display it
	
	uint8_t dayOfWeek = myTime.getDayOfWeek();
	
	if (dayOfWeek > 7)
		dayOfWeek = 0;
		
	Serial.print(daysOfWeek[dayOfWeek]);
	
	Serial.print(F(" "));
	
	uint8_t month = myTime.getMonth();
	
	if (month > 12)
		month = 0;
		
	Serial.print(monthNames[month]);
	
	Serial.print(F(" "));
	
	Serial.print(myTime.getDayOfMonth());
	
	Serial.print(F(", "));
	
	Serial.print(2000 + myTime.getYear());
	
	Serial.print(F(" - "));
	
	Serial.print(myTime.getHours());
	
	Serial.print(F(":"));
	
	if (myTime.getMinutes() < 10)
		Serial.print(F("0"));
	
	Serial.print(myTime.getMinutes());
	
	Serial.print(F(":"));
	
	if (myTime.getSeconds() < 10)
		Serial.print(F("0"));
		
	Serial.println(myTime.getSeconds());
}

/////////////////////////////////////////////
// Show the Chronodot's configuration data //
/////////////////////////////////////////////

void showStatus() {
	Serial.println(F(""));

	bool batteryOscillator = Chronodot::getOscillatorEnabled();
	
	Serial.print(F("Oscillator enabled on battery is "));
	Serial.println(batteryOscillator);

	bool batterySquarewave = Chronodot::getBatterySquareWaveEnabled();
	
	Serial.print(F("Battery backed square wave enabled is "));
	Serial.println(batterySquarewave);
	
	uint8_t speed = Chronodot::getOutputSquarewaveSpeed();
	
	switch (speed) {
		case SPEED_1_HZ:
			Serial.println(F("Square wave output is 1 Hz"));
			break;
		case SPEED_1_024_KHZ:
			Serial.println(F("Square wave output is 1.024 kHz"));
			break;
		case SPEED_4_096_KHZ:
			Serial.println(F("Square wave output is 4.096 kHz"));
			break;
		case SPEED_8_192_KHZ:
			Serial.println(F("Square wave output is 8.192 kHz"));
			break;
		default:
			Serial.println(F("Square wave off, interupt on alarm"));
	}
	
	bool thirtyTwo = Chronodot::get32kHzOutputEnabled();
	
	Serial.print(F("32kHz otuput enabled is "));
	Serial.println(thirtyTwo);
	
	bool oscillatorStop = Chronodot::getOscillatorStop();
	
	Serial.print(F("Oscillator was stopped is "));
	Serial.println(oscillatorStop);
	
	bool busy = Chronodot::isBusy();
	
	Serial.print(F("Busy flag is "));
	Serial.println(busy);
	
	bool alarm = Chronodot::getAlarmOneEnabled();
	
	Serial.print(F("Alarm one enabled is "));
	Serial.println(alarm);
	
	alarm = Chronodot::getAlarmOneFired();
	
	Serial.print(F("Alarm one fired is "));
	Serial.println(alarm);
	
	alarm = Chronodot::getAlarmTwoEnabled();
	
	Serial.print(F("Alarm two enabled is "));
	Serial.println(alarm);
	
	alarm = Chronodot::getAlarmTwoFired();
	
	Serial.print(F("Alarm two fired is "));
	Serial.println(alarm);
	
	int8_t agingOffset = Chronodot::getAgingOffset();
	
	Serial.print(F("Aging offset is "));
	Serial.println(agingOffset);
	
	float temp = Chronodot::getTemperature();
	
	Serial.print(F("Temperature is "));
	Serial.print(temp);
	Serial.println(F(" C"));
}

///////////////////////////////
// Modifies the current time //
///////////////////////////////

void changeTime() {
	Serial.println(F(""));
	Serial.println(F("Please enter the time in the form:"));
	Serial.println(F(""));
	Serial.println(F("7, 1/24/05 23:18:12 d"));
	Serial.println(F(""));
	Serial.println("The first digit is the day (1 = Sunday, 7 = Saturday),");
	Serial.println(F("use two digits for the year, and use 24h time."));
	Serial.println(F("The 'd' is how we know you're done typing."));
	Serial.println(F(""));
	Serial.println(F("Enter 'x' to return to the main menu."));

	while (true) {
		Serial.println(F(""));
		Serial.print(F("? "));
	
		// We're going to gather up characters until we get an 'x', 'd', or buffer is full
		
		uint8_t result = getLineIntoBuffer();
		
		if (result == EXIT_MENU)
			return;
		else if (result == TOO_LONG)
			continue;
	
		// We've got our string, we're going to try to parse it out
		
		uint8_t numbers[7];
		
		uint8_t start = 0;
		uint8_t nextStart = 0;
		
		for (uint8_t i = 0; i < 7; i++) {
			numbers[i] = getNextNumber(buffer, start, &nextStart);
			
			if (nextStart == 255 && i != 6) {
				Serial.println(F(""));
				Serial.println(F("Didn't get enough numbers"));
				
				continue;	// They messed up, let them try again
			}
			
			start = nextStart;
		}
		
		// OK, numbers contains all the fields. Do some quick validation
		
		if (numbers[0] == 0 || numbers[0] > 7) {
			Serial.println(F(""));
			Serial.println(F("Day of week out of range"));
			
			continue;
		} else if (numbers[1] == 0 || numbers[1] > 12) {
			Serial.println(F(""));
			Serial.println(F("Month out of range"));
			
			continue;
		} else if (numbers[2] == 0 || numbers[2] > 31) {
			Serial.println(F(""));
			Serial.println(F("Day of month out of range"));
			
			continue;
		} else if (numbers[4] >= 24) {
			Serial.println(F(""));
			Serial.println(F("Hours out of range"));
			
			continue;
		} else if (numbers[5] >= 59) {
			Serial.println(F(""));
			Serial.println(F("Minutes out of range"));
			
			continue;
		} else if (numbers[6] >= 59) {

			Serial.println(F(""));
			Serial.println(F("Seconds out of range"));
			
			continue;
		}
		
		// OK, that's it. Update our time.
		
		myTime.setDayOfWeek(numbers[0]);
		myTime.setMonth(numbers[1]);
		myTime.setDayOfMonth(numbers[2]);
		myTime.setYear(numbers[3]);
		myTime.setHours(numbers[4]);
		myTime.setMinutes(numbers[5]);
		myTime.setSeconds(numbers[6]);
		
		Chronodot::setTime(&myTime);
	
		// Show them the result, go back to main menu
	
		Serial.println(F(""));
		Serial.print(F("Time is now: "));
		
		showTime();
		
		return;
	}
}

uint8_t getLineIntoBuffer() {
	uint8_t i = 0;
		
	while (true) {
		while (Serial.available() == 0) {
			// Just wait
		}
		
		buffer[i] = Serial.read();
		
		Serial.print(buffer[i]);
		
		if (buffer[i] == 8) {
			// Delete character, move back one
			
			i = i - 2;
		} else if (buffer[i] == 'd' || buffer[i] == 'D') {
			buffer[i + 1] = 0;
			
			return GOT_STRING;	// Got what we need
		} else if (buffer[i] == 'x' || buffer[i] == 'X') {
			return EXIT_MENU;	// Back to the main menu
		} else if (i >= 99) {
			Serial.println(F(""));
			Serial.println(F("Input too long."));
			
			return TOO_LONG;
		}
		
		i++;
	}
}

uint8_t getNextNumber(char *data, uint8_t start, uint8_t *nextStart) {
	uint8_t working = 0;
	
	bool foundNumbers = false;
	
	for (uint8_t i = start; i < 255; i++) {
		
		if (data[i] >= '0' && data[i] <= '9') {
			foundNumbers = true;
			working = working * 10 + (data[i] - '0');
		} else if (data[i] == 0 || data[i] == 'd' || data[i] == 'D') {
			// End of the string
			
			*nextStart = 255;
			
			return working;
		} else if (!foundNumbers) {
			// We're just eating up characters before we start doing things
		} else {
			// We're done with the numbers. Tell the caller where the next data may start.
			
			*nextStart = i;
			
			// Return what we got
			
			return working;
		}
	}
	
	return working;	// Shouldn't get here in correct operation
}

//////////////////////////////////
// Alarm one & two edit/display //
//////////////////////////////////

void alarmMenu() {

}

//////////////////////////
// Alarm based settings //
//////////////////////////

void alarmSettings() {
	while (true) {
		Serial.println(F(""));
		Serial.println(F("Alarm Settings"));
		Serial.println(F("---------"));
		Serial.println(F("1. Toggle alarm one enabled"));
		Serial.println(F("2. Toggle alarm two enabled"));	
		Serial.println(F("3. Reset alarm one flag"));
		Serial.println(F("4. Reset alarm two flag"));
		Serial.println(F(""));
		Serial.println(F("x. Back to main menu"));
		Serial.println(F(""));
	
		lastInput = 0;
	
		while (lastInput == 0) {
			Serial.print(F("? "));
		
			while (Serial.available() == 0) {
				// Just wait
			}
		
			lastInput = Serial.read();

			Serial.println(lastInput);

			while (Serial.available() > 0)	// Eat up any other input
				Serial.read();
			
			Serial.println(F(""));
			
			bool old;		// Not allowed in the switch with this compiler
			
			switch (lastInput) {
				case '1':
					old = Chronodot::getAlarmOneEnabled();
					
					Chronodot::enableAlarmOne(!old);
					
					Serial.print(F("Alarm one enabled is now "));
					Serial.println(!old);
					
					break;
				case '2':
					old = Chronodot::getAlarmTwoEnabled();
					
					Chronodot::enableAlarmTwo(!old);
					
					Serial.print(F("Alarm two enabled is now "));
					Serial.println(!old);
					
					break;
				case '3':
					Chronodot::resetAlarmOneFired();
					
					Serial.println(F("Alarm one flag reset"));
					
					break;
				case '4':
					Chronodot::resetAlarmTwoFired();
					
					Serial.println(F("Alarm two flag reset"));

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

///////////////////////////
// General settings menu //
///////////////////////////

void generalSettings() {
	while (true) {
		Serial.println(F(""));
		Serial.println(F("General Settings"));
		Serial.println(F("---------"));
		Serial.println(F("1. Toggle battery oscillator"));
		Serial.println(F("2. Toggle battery square wave"));	
		Serial.println(F("3. Trigger temperature reading"));
		Serial.println(F("4. Set interrupt on alarm"));
		Serial.println(F("5. Output 1 Hz square wave"));
		Serial.println(F("6. Output 1.024 kHz square wave"));
		Serial.println(F("7. Output 4.096 kHz square wave"));
		Serial.println(F("8. Output 8.196 kHz square wave"));
		Serial.println(F("9. Reset oscillator stop"));
		Serial.println(F("0. Toggle 32 kHz out"));
		Serial.println(F(""));
		Serial.println(F("x. Back to main menu"));
		Serial.println(F(""));
	
		lastInput = 0;
	
		while (lastInput == 0) {
			Serial.print(F("? "));
		
			while (Serial.available() == 0) {
				// Just wait
			}
		
			lastInput = Serial.read();

			Serial.println(lastInput);

			while (Serial.available() > 0)	// Eat up any other input
				Serial.read();
			
			Serial.println(F(""));
			
			bool old;		// Not allowed in the switch with this compiler
			
			switch (lastInput) {
				case '1':
					old = Chronodot::getOscillatorEnabled();
					
					Chronodot::enableOscillator(!old);
					
					Serial.print(F("Battery oscillator is now "));
					Serial.println(!old);
					
					break;
				case '2':
					old = Chronodot::getBatterySquareWaveEnabled();
					
					Chronodot::enableBatterySquareWave(!old);
					
					Serial.print(F("Battery square wave is now "));
					Serial.println(!old);
					
					break;
				case '3':
					Chronodot::senseTemperature(true);
					
					Serial.println(F("Temperature reading started"));

					Serial.print(F("Temperature is "));
					Serial.print(Chronodot::getTemperature());
					Serial.println(F(" C"));
					
					break;
				case '4':
					Chronodot::outputInterruptOnAlarm();
					
					Serial.println(F("Interrupt on alarm set"));

					break;
				case '5':
					Chronodot::outputSquarewaveSpeed(SPEED_1_HZ);
					
					Serial.println(F("Squarewave output is 1 Hz"));

					break;
				case '6':
					Chronodot::outputSquarewaveSpeed(SPEED_1_024_KHZ);
					
					Serial.println(F("Squarewave output is 1.024 kHz"));

					break;				
				case '7':
					Chronodot::outputSquarewaveSpeed(SPEED_4_096_KHZ);
					
					Serial.println(F("Squarewave output is 4.096 kHz"));

					break;				
				case '8':
					Chronodot::outputSquarewaveSpeed(SPEED_8_192_KHZ);
					
					Serial.println(F("Squarewave output is 8.192 kHz"));

					break;				
				case '9':
					Chronodot::resetOscillatorStop();
					
					Serial.println(F("Oscillator stop flag reset"));

					break;
				case '0':
					old = Chronodot::get32kHzOutputEnabled();
					
					Chronodot::enable32kHzOutput(!old);
					
					Serial.print(F("32kHz output enabled is "));
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
