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

// Debounce settings
unsigned long lastDebounceTime = 0;  // Last time a sensor was triggered
unsigned long debounceDelay = 100;   // Debounce interval (milliseconds), decreased for quicker responsiveness

// Loading screen settings
bool loadingScreen = true;
bool displayTimeOnly = false;  // Flag to control whether to display time only

void setup() {
  // Start Leanbot
  Leanbot.begin();

  // Start OLED display
  u8g2.setBusClock(400000);                 // I2C speed 400kHz
  u8g2.begin();
  
  // Set initial time (hours, minutes, seconds, day, month, year)
  setTime(15, 11, 24, 27, 9, 2023);

  // Display loading screen for a brief period
  showLoadingScreen();
}

void loop() {
  if (loadingScreen) {
    showLoadingScreen(); // Keep showing loading screen during startup
    return;
  }

  u8g2.setFont(u8g2_font_profont29_tf);
  u8g2.firstPage();
  
  do {
    // Display greeting based on time of day (this will always be shown)
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.setCursor(0, 10);
    u8g2.print(getGreeting());

    // Only display the current setting when 'displayTimeOnly' is false
    if (!displayTimeOnly) {
      // Display the current setting and value on the OLED
      u8g2.setFont(u8g2_font_5x8_tr);
      u8g2.setCursor(0, 20);  // Adjusted vertical position to avoid overlap
      u8g2.print("Setting: ");
      
      // Display current setting name
      u8g2.setCursor(60, 20);  // Shifted the setting name to the right for better spacing
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
          u8g2.print("Day");
          break;
        case MONTH_MODE:
          u8g2.print("Month");
          break;
        case YEAR_MODE:
          u8g2.print("Year");
          break;
      }
    }

    // Center the time text and move it slightly up
    u8g2.setFont(u8g2_font_profont22_tf);        
    String timeString = String(hour() < 10 ? "0" : "") + String(hour()) + ":" +
                        String(minute() < 10 ? "0" : "") + String(minute()) + ":" +
                        String(second() < 10 ? "0" : "") + String(second());
    int textWidth = u8g2.getStrWidth(timeString.c_str());  // Get the width of the time text
    int centerX = (128 - textWidth) / 2;  // 128 is the screen width
    int centerY = 45;  // Default vertical position for time
    
    // Adjusting vertical position slightly upwards
    centerY -= 5;  // Move the text 5 pixels upwards

    u8g2.setCursor(centerX, centerY);  // Set the adjusted position
    u8g2.print(timeString);

    // Display the current date
    u8g2.setFont(u8g2_font_profont22_tf);
    u8g2.setCursor(10, 60);  // Adjusted vertical position for date
    u8g2.print(day());
    u8g2.print("|");
    u8g2.print(month());
    u8g2.print("|");
    u8g2.print(year());
    
  } while (u8g2.nextPage());

  unsigned long currentMillis = millis();

  // Sensor Inputs for Adjusting Time/Date with debounce logic
  if ((LbTouch.read(TB1A) == HIGH) && (currentMillis - lastDebounceTime > debounceDelay)) {  // Increment hour/minute/second/date
    adjustTime(true);
    lastDebounceTime = currentMillis;  // Update last debounce time
  }
  if ((LbTouch.read(TB2A) == HIGH) && (currentMillis - lastDebounceTime > debounceDelay)) {  // Decrement hour/minute/second/date
    adjustTime(false);
    lastDebounceTime = currentMillis;  // Update last debounce time
  }
  if ((LbTouch.read(TB1B) == HIGH) && (currentMillis - lastDebounceTime > debounceDelay)) {  // Switch to next setting
    switchSetting();
    lastDebounceTime = currentMillis;  // Update last debounce time
  }
  if ((LbTouch.read(TB2B) == HIGH) && (currentMillis - lastDebounceTime > debounceDelay)) {  // Toggle display (show time without setting text)
    displayTimeOnly = !displayTimeOnly;  // Toggle the flag to show/hide setting text
    lastDebounceTime = currentMillis;  // Update last debounce time
  }
  
  LbDelay(200);  // Additional debounce delay for sensor presses (optional)
}


void showLoadingScreen() {
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 30);
    u8g2.print("Loading...");
  } while (u8g2.nextPage());
  
  delay(2000); // Wait for 2 seconds before transitioning to the main UI
  loadingScreen = false;
}

String getGreeting() {
  int currentHour = hour();
  
  // Ensure the RGB LED is cleared and set its brightness
  LbRGB.clear();
  LbRGB.setBrightness(240);

  if (currentHour < 12) { // Morning
    LbRGB[ledA] = 0xFFFF00;  // Yellow
    LbRGB.show();
    return "Good Morning!";
  } else if (currentHour < 18) { // Afternoon
    LbRGB[ledA] = 0xFFA500;  // Orange
    LbRGB.show();
    return "Good Afternoon!";
  } else { // Night
    LbRGB[ledA] = 0x0000FF;  // Blue
    LbRGB.show();
    return "Good Evening!";
  }
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

void printDigits(int digits) {
  // Utility function for digital clock display: prints leading 0 if needed
  u8g2.print(":");
  if (digits < 10) {
    u8g2.print('0');
  }
  u8g2.print(digits);
}
