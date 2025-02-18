#include <Leanbot.h>          // use Leanbot library
#include <U8g2lib.h>         // U8g2 Library Reference
#include <TimeLib.h>          // Reference for time manipulation

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);   // SH1106 for OLED display

// Time Setting Mode
enum TimeSettingMode {
  HOUR_MODE,
  MINUTE_MODE,
  SECOND_MODE,
  DATE_MODE,
  MONTH_MODE,
  YEAR_MODE
};

TimeSettingMode currentMode = HOUR_MODE;  // Start with setting hour

void setup() {
  // Start Leanbot
  Leanbot.begin();

  // Start OLED display
  u8g2.setBusClock(400000);                 // I2C speed 400kHz
  u8g2.begin();
  
  // Set initial time (hours, minutes, seconds, day, month, year)
  setTime(15, 11, 24, 27, 9, 2023);
}

void loop() {
  u8g2.setFont(u8g2_font_profont29_tf);
  u8g2.firstPage();
  
  do {
    // Display the current setting and value on the OLED
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.setCursor(0, 10);
    u8g2.print("Setting: ");
    
    // Display current setting name
    switch (currentMode) {
      case HOUR_MODE:
        u8g2.print("Hour");
        break;
      case MINUTE_MODE:
        u8g2.print("Minute");
        break;
      case SECOND_MODE:
        u8g2.print("Second");
        break;
      case DATE_MODE:
        u8g2.print("Date");
        break;
      case MONTH_MODE:
        u8g2.print("Month");
        break;
      case YEAR_MODE:
        u8g2.print("Year");
        break;
    }

    // Display the current time and date
    u8g2.setFont(u8g2_font_profont29_tf);        
    u8g2.setCursor(0, 60);  
    u8g2.print(hour());
    printDigits(minute());
    printDigits(second());

    u8g2.setFont(u8g2_font_profont22_tf);
    u8g2.setCursor(10, 25);
    u8g2.print(day());
    u8g2.print("|");
    u8g2.print(month());
    u8g2.print("|");
    u8g2.print(year());
    
  } while (u8g2.nextPage());

  // Sensor Inputs for Adjusting Time/Date
  if (LbTouch.read(TB1A) == HIGH) {  // Increment hour/minute/second/date
    adjustTime(true);
  }
  if (LbTouch.read(TB1B) == HIGH) {  // Decrement hour/minute/second/date
    adjustTime(false);
  }
  if (LbTouch.read(TB2A) == HIGH) {  // Switch to next setting (hour -> minute -> second -> date -> month -> year)
    switchSetting();
  }
  if (LbTouch.read(TB2B) == HIGH) {  // Switch to previous setting
    previousSetting();
  }
  
  LbDelay(200);  // Debounce delay for sensor presses
}

void adjustTime(bool increment) {
  switch (currentMode) {
    case HOUR_MODE:
      (increment) ? setTime(hour() + 1, minute(), second(), day(), month(), year()) : setTime(hour() - 1, minute(), second(), day(), month(), year());
      break;
    case MINUTE_MODE:
      (increment) ? setTime(hour(), minute() + 1, second(), day(), month(), year()) : setTime(hour(), minute() - 1, second(), day(), month(), year());
      break;
    case SECOND_MODE:
      (increment) ? setTime(hour(), minute(), second() + 1, day(), month(), year()) : setTime(hour(), minute(), second() - 1, day(), month(), year());
      break;
    case DATE_MODE:
      (increment) ? setTime(hour(), minute(), second(), day() + 1, month(), year()) : setTime(hour(), minute(), second(), day() - 1, month(), year());
      break;
    case MONTH_MODE:
      (increment) ? setTime(hour(), minute(), second(), day(), month() + 1, year()) : setTime(hour(), minute(), second(), day(), month() - 1, year());
      break;
    case YEAR_MODE:
      (increment) ? setTime(hour(), minute(), second(), day(), month(), year() + 1) : setTime(hour(), minute(), second(), day(), month(), year() - 1);
      break;
  }
}

void switchSetting() {
  // Move to the next setting (cycle through hour, minute, second, date, month, year)
  if (currentMode == YEAR_MODE) {
    currentMode = HOUR_MODE;
  } else {
    currentMode = static_cast<TimeSettingMode>(currentMode + 1);
  }
}

void previousSetting() {
  // Move to the previous setting (cycle in reverse order)
  if (currentMode == HOUR_MODE) {
    currentMode = YEAR_MODE;
  } else {
    currentMode = static_cast<TimeSettingMode>(currentMode - 1);
  }
}

void printDigits(int digits) {
  // Utility function for digital clock display: prints leading 0 if needed
  u8g2.print(":");
  if (digits < 10) {
    u8g2.print('0');
  }
  u8g2.print(digits);
}
