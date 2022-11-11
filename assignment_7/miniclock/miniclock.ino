#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Variables to store the time
int hours ;
int minutes ;
int seconds ;

int analogPin = A0;
int val = 0;

int ledred = 8;
int ledgreen = 10;

// Constants for the button pins
int countMinutes = 0;
int countHours = 0;

int timer_round = 0;

int buzzer = 12;

int alarmstate = 0;

int eepromshowminutes ;
int eepromshowhours ; 

int hoursfromeeprom ;
int minutesfromeeprom ;

const int PIN_BUTTON_MODE = 3;
const int PIN_BUTTON_SET = 2;
const int PIN_BUTTON_ALARMSET = 4;

const int PIN_BUTTON_ALARMSTOP = 6;

const int BUTTON_MODE_DEBOUNCE_TIME = 250;
const int BUTTON_SET_DEBOUNCE_TIME = 100;

const int MODE_SHOW_TIME = 0;
const int MODE_SET_SECONDS = 3;
const int MODE_SET_MINUTES = 2;
const int MODE_SET_HOURS = 1;

int timeformuser;

// Variables for the button state
// We are using the internal pull-up resistors via INPUT_PULLUP, so
// press is LOW and not pressed is HIGH
unsigned long elapsedButtonModeMillis = 0;
unsigned long previousButtonModeMillis = 0;

unsigned long elapsedButtonSetMillis1 = 0;
unsigned long previousButtonModeMillis1 = 0;



unsigned long elapsedButtonSetMillis = 0;
unsigned long previousButtonSetMillis = 0;

// Char array for the time being showed on the display
char timeString[9];

// Variables to store the time
unsigned long currentMillis = 0;

// Int is enough to store the elapsed time
int elapsedTimeUpdateMillis = 0;
unsigned long previousTimeUpdateMillis = 0;

float percentageOfSecondElapsed = 0;

byte currentMode = MODE_SHOW_TIME;

int alarm_hours_set = -1;
int alarm_minutes_set = -1;

unsigned long previousMillis = 0;
const long interval = 500;
int buzzerstate = LOW;

bool checkbuzzer;

int showhours;
int showminutes;

char checkinput;

// A complete list of all displays is available at: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void setup(void) {

  Serial.begin(9600);
  u8g2.setFont(u8g2_font_logisoso28_tf);
  u8g2.begin();

  // Configure the pins of the buttons with the internal PULLUP resistor
  pinMode(PIN_BUTTON_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_SET, INPUT_PULLUP);
  pinMode(PIN_BUTTON_ALARMSET, INPUT_PULLUP);
  pinMode(PIN_BUTTON_ALARMSTOP, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(analogPin, INPUT);

  // EEPROM.write(0, 0);

  // EEPROM.write(2, 0);

  // EEPROM.write(4, 0);
  // EEPROM.write(6, 0);
  
  // EEPROM.write(8, 0);

  hours = EEPROM.read(0);
  minutes = EEPROM.read(2);
 
  checkbuzzer = EEPROM.read(4);


}

void loop(void) {

  int sensorValue = analogRead(A0);
  val = analogRead(analogPin); //อ่านค่า

  
  if(val < 500){
    digitalWrite(ledgreen, HIGH);
  }else{
    digitalWrite(ledgreen, LOW);
  }

    if(val > 500){
    digitalWrite(ledred, HIGH);
  }else{
    digitalWrite(ledred, LOW);
  }



  // millis() itself takes 1.812 micro seconds that is 0.001812 milli seconds
  // https://arduino.stackexchange.com/questions/113/is-it-possible-to-find-the-time-taken-by-millis
  currentMillis = millis();
  if (digitalRead(PIN_BUTTON_ALARMSTOP) == LOW){
    stopalarm();
   alarm_hours_set = -1;
   alarm_minutes_set = -1;
  }

  if (digitalRead(PIN_BUTTON_MODE) == LOW) {

    buttonModeHandler();
  }

  if (digitalRead(PIN_BUTTON_SET) == LOW) {

    buttonSetHandler();

  }

  if (digitalRead(PIN_BUTTON_ALARMSET) == LOW) {
    alarmSetHoursAndMinutes();
   
  }


  alert();
  drawTime();
  checkTime();




  if (currentMode == MODE_SHOW_TIME) {
    increaseSeconds();
  } else {
    previousTimeUpdateMillis = currentMillis;
  }


  drawScreen();
}

void checkTime() {
  // Check if a minutes has been elapsed
  if (seconds > 59) {
    seconds = 0;
    minutes++;
    EEPROM.update(2, minutes);

  }

  // Check if an hour has been elapsed
  if (minutes > 59) {
    minutes = 0;
    EEPROM.update(2, minutes);
    hours++;
    EEPROM.update(0, hours);

  }

  // Check if a day has been elapsed
  if (hours > 23) {
    hours = 0;
    EEPROM.update(0, hours);

  }
}

void buttonModeHandler() {
  elapsedButtonModeMillis = currentMillis - previousButtonModeMillis;
  if (elapsedButtonModeMillis > BUTTON_MODE_DEBOUNCE_TIME) {
    //Serial.println("Mode Handler");
    previousButtonModeMillis = currentMillis;
    currentMode++;

    if (currentMode > 3) {
      currentMode = 0;
    }
  }
}

void buttonSetHandler() {
  elapsedButtonSetMillis = currentMillis - previousButtonSetMillis;
  //Serial.println(elapsedButtonSetMillis);
  if (elapsedButtonSetMillis > BUTTON_SET_DEBOUNCE_TIME) {
    //Serial.println("Set Handler");
    previousButtonSetMillis = currentMillis;

    if (currentMode == MODE_SET_SECONDS) {
      seconds = 0;
    }
    if (currentMode == MODE_SET_MINUTES) {
      minutes++;
      EEPROM.update(2, minutes);
    }
    if (currentMode == MODE_SET_HOURS) {
      hours++;
      EEPROM.update(0, hours);

    }
  }
  
}

void increaseSeconds() {
  elapsedTimeUpdateMillis = currentMillis - previousTimeUpdateMillis;

  // Check if 1000ms, 1 second, has been elapsed
  if (elapsedTimeUpdateMillis > 1000) {
    seconds++;

    // It might be possible that more than 1000ms has been elapsed e.g. 1200ms
    // Then there are already 200ms elapsed of the next second. We need to
    // substract these on the "last time". So the next second will be updated 200ms earlier.
    // This reduces the amount of time drift.
    previousTimeUpdateMillis = currentMillis - (elapsedTimeUpdateMillis - 1000);
  }
}

void drawScreen() {
  u8g2.firstPage();

  do {

    if (currentMode != MODE_SHOW_TIME) {
      u8g2.drawTriangle((currentMode - 1) * 43 + 5, 0, currentMode * 43 - 5, 0, (currentMode - 1) * 43 + 21, 5);
    }

    drawAnimation();
    drawTime();

  } while (u8g2.nextPage());
}

void drawTime() {
  
  // Found at https://forum.arduino.cc/index.php?topic=371117.0
  // sprintf_P uses the Program Memory instead of RAM, more info at http://gammon.com.au/progmem
  // Here we format the minutes and seconds with a leading zero: e.g. 01, 02, 03 etc.
  sprintf_P(timeString, PSTR("%2d:%02d:%02d"), hours, minutes, seconds);
  

  // Draw the timeString
  u8g2.drawStr(0, 45, timeString);
}

void drawAnimation() {
  // Calculate the percentage elapsed of a second
  percentageOfSecondElapsed = elapsedTimeUpdateMillis / 1000.0;

  if (currentMode == MODE_SHOW_TIME) {
    // Draw the yellow lines
    u8g2.drawBox(0, 0, 127 - (127 * percentageOfSecondElapsed), 2);
    u8g2.drawBox(0, 3, (127 * percentageOfSecondElapsed), 2);
  }
}

void alarmSetHoursAndMinutes() {
  Serial.flush();
  Serial.println("Please enter hours: ");

  while (alarm_hours_set == -1) {
    if (Serial.available() > 0) {
      alarm_hours_set = Serial.readString().toInt();
      
      if (alarm_hours_set <= 23 && alarm_hours_set > 0  ) {
        Serial.print("You pressed ");
        Serial.println(alarm_hours_set);
        EEPROM.put(6, alarm_hours_set);
        EEPROM.get(6, hoursfromeeprom);
        EEPROM.put(4, true);

      } else {
        Serial.println("Error, Please press the button again.");
      }
    }
  }
  Serial.println("Please enter minutes: ");

  while (alarm_minutes_set == -1) {
    if (Serial.available() > 0) {
      alarm_minutes_set = Serial.readString().toInt();
      if (alarm_minutes_set <= 59 && alarm_minutes_set > 0) {
        Serial.print("You pressed ");
        Serial.println(alarm_minutes_set);
        EEPROM.put(8, alarm_minutes_set);
        EEPROM.get(8, minutesfromeeprom);
        Serial.print("Alarm is set at ");
        if(alarm_hours_set < 10 && alarm_hours_set >= 0){
          Serial.print("0");
          Serial.print(alarm_hours_set);
          Serial.print(":");
        }else{
          Serial.print(alarm_hours_set);
          Serial.print(":");
        }

        if(alarm_minutes_set < 10 && alarm_minutes_set >= 0){
          Serial.print("0");
          Serial.println(alarm_minutes_set);


         
        }else{
          Serial.print(alarm_minutes_set);


          
        }


        



      } else {
        Serial.println("Error, Please press the button again.");
      }
    }
  }



  alarm_hours_set = -1;
  alarm_minutes_set = -1;
}

void alert() {
  EEPROM.get(6, hoursfromeeprom);
  EEPROM.get(8, minutesfromeeprom);

  EEPROM.get(4, checkbuzzer);

  unsigned long currentMillis = millis();

  if (hours == hoursfromeeprom && minutes == minutesfromeeprom && checkbuzzer) {
    if (timer_round < 100) {

      if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (buzzerstate == LOW) {
          buzzerstate = HIGH;

          
        } else {
          
          buzzerstate = LOW;
          timer_round++;
          
        }

        // set the LED with the ledState of the variable:
        digitalWrite(buzzer, buzzerstate);
      }
    }
  } else {
    timer_round = 0;
    digitalWrite(buzzer, LOW);


  }
  
}

void stopalarm(){
  alarm_hours_set = 3;
  alarm_minutes_set = -3;
  EEPROM.put(4, false);
  EEPROM.put(6, alarm_hours_set);
  EEPROM.put(8, alarm_minutes_set);
  EEPROM.get(6, hoursfromeeprom);
  EEPROM.get(8, minutesfromeeprom);
  Serial.println("Reset......");
}

