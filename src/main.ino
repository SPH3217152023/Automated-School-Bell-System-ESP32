#include <WiFi.h>
#include "time.h"
#include <SPI.h>
#include <Wire.h>

// WIFI
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// TIME
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 3600;

// PINS
#define VIB_SENSOR 34
#define BTN_UP 27
#define BTN_DOWN 26
#define BTN_SEL 33

// STATES
enum ScreenState { HOME, MENU, EDIT };
ScreenState screen = HOME;

int menuIndex = 0;

// MENU ITEMS
String menuItems[] = {
  "Set Time",
  "Lesson Start",
  "Break Time",
  "Assembly",
  "Closing"
};

// SCHEDULE
String schedule[] = {"08:00","10:00","13:00","16:00"};
int numEvents = 4;

// EDIT VARIABLES
int editHour = 8;
int editMinute = 0;
bool editHourSelected = true;

// TIMING
unsigned long lastTrigger = 0;

// -------- TIME --------
String getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--:--";
  char buf[6];
  strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
  return String(buf);
}

// -------- NEXT EVENT --------
String getNextEvent(String now) {
  for (int i = 0; i < numEvents; i++) {
    if (schedule[i] > now) return schedule[i];
  }
  return schedule[0];
}

// -------- BUTTON (DEBOUNCE) --------
bool pressed(int pin) {
  static unsigned long lastPress[40];
  if (digitalRead(pin) == LOW && millis() - lastPress[pin] > 250) {
    lastPress[pin] = millis();
    return true;
  }
  return false;
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(VIB_SENSOR, INPUT);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SEL, INPUT_PULLUP);

  // INIT DISPLAY FROM BRAVINE MODULE
  initDisplay();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  configTime(gmtOffset_sec, 0, ntpServer);
}

// -------- LOOP --------
void loop() {
  String now = getTime();

  handleButtons();

  // VIBRATION TRIGGER
  int vib = analogRead(VIB_SENSOR);
  if (vib > 1500 && millis() - lastTrigger > 5000) {
    ringBell();   // FROM BRAVINE
    lastTrigger = millis();
  }

  // AUTO SCHEDULE
  if (screen == HOME) {
    for (int i = 0; i < numEvents; i++) {
      if (now == schedule[i]) {
        ringBell();   // FROM BRAVINE
        delay(60000);
      }
    }
  }

  // DISPLAY FROM BRAVINE
  showStatus("IDLE", now, getNextEvent(now));

  delay(100);
}

// -------- BUTTON LOGIC --------
void handleButtons() {

  if (screen == HOME) {
    if (pressed(BTN_SEL)) screen = MENU;
  }

  else if (screen == MENU) {

    if (pressed(BTN_UP)) menuIndex--;
    if (pressed(BTN_DOWN)) menuIndex++;

    if (menuIndex < 0) menuIndex = 4;
    if (menuIndex > 4) menuIndex = 0;

    if (pressed(BTN_SEL)) {
      screen = EDIT;

      if (menuIndex > 0) {
        editHour = schedule[menuIndex - 1].substring(0,2).toInt();
        editMinute = schedule[menuIndex - 1].substring(3,5).toInt();
      }

      editHourSelected = true;
    }
  }

  else if (screen == EDIT) {

    if (pressed(BTN_SEL)) {
      if (editHourSelected) {
        editHourSelected = false;
      } else {
        if (menuIndex > 0) {
          char buf[6];
          sprintf(buf, "%02d:%02d", editHour, editMinute);
          schedule[menuIndex - 1] = String(buf);
        }
        screen = MENU;
      }
    }

    if (editHourSelected) {
      if (pressed(BTN_UP)) editHour = (editHour + 1) % 24;
      if (pressed(BTN_DOWN)) editHour = (editHour - 1 + 24) % 24;
    } else {
      if (pressed(BTN_UP)) editMinute = (editMinute + 1) % 60;
      if (pressed(BTN_DOWN)) editMinute = (editMinute - 1 + 60) % 60;
    }
  }
}
