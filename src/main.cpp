#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Joystick.h>

// the Joystick lib can be found here:
// https://github.com/MHeironimus/ArduinoJoystickLibrary?utm_source=platformio&utm_medium=piohome
// unfortunately, it doesn't support ffb.

#define STANDBY 0
#define TESTING 1
#define STOP    2

const int LDR = A0;
const int LED = 13;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int LDR_value = 0;
int startTime;
int currentTime;
int triggeredTime;
int testNumber = 0;
bool wasTriggered;
char outputStr1[16];
char outputStr2[16];
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 32, 0,
  true, true, false, false, false, false,
  true, true, false, false, false);

int mode = STANDBY;

// forward function declarations:
int readLDR();

// function impelentation

void setup() 
{
  Joystick.begin();
  pinMode(LED, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Setup:");

  startTime = 0;
  currentTime = 0;
  triggeredTime = 0;
  wasTriggered = false;
}

void loop() 
{
  LDR_value = readLDR();

  delay(1);
}

int readLDR()
{
  // - Every 5 seconds, trigger the first button on the controller.
  // - Then check to see if the photoresistore has a change of 200 units - this indicates
  //   that the app has displayed a white square on screen.
  int value = analogRead(LDR);
  currentTime = millis();
  if (mode == STANDBY)
  {
    if (currentTime - startTime >= 5000) // attempt the test every 5 seconds
    {
      mode = TESTING;
      startTime = currentTime;
      Joystick.pressButton(0);
      testNumber++;
      sprintf(outputStr1, "Test #: %-5d", testNumber);
    }
  }
  if (mode == TESTING)
  {
    sprintf(outputStr2, "T: %-10d", currentTime - startTime);
    if (value < 500)
    {
      digitalWrite(LED, LOW);
      wasTriggered = false;
    }
    else
    {
      digitalWrite(LED, HIGH);
      triggeredTime = currentTime;
      wasTriggered = true;
    }

    if (wasTriggered)
    {
      int deltaTime = triggeredTime - startTime;
      char buffer[16];
      sprintf(buffer, "%-16d", deltaTime);
      strcpy(outputStr2, buffer);
      wasTriggered = false;
      triggeredTime = 0;
      startTime = currentTime;
      mode = STANDBY;
      Joystick.releaseButton(0);
    }

    if ((currentTime - startTime) > 10000) // timeout
    {
      mode = STANDBY;
      sprintf(outputStr2, "%-15s", "Fail! Timeout--");
      triggeredTime = 0;
      startTime = currentTime;
      Joystick.releaseButton(0);
    }

  }

  lcd.setCursor(0, 0);
  lcd.print(outputStr1);
  lcd.setCursor(0, 1);
  lcd.print(outputStr2);
  
  return value;
}