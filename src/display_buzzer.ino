#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUZZER 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize display
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
}

// Show system status
void showStatus(String mode, String timeNow, String nextEvent) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Mode: " + mode);
  display.println("Time: " + timeNow);
  display.println("Next: " + nextEvent);
  display.display();
}

// Ring buzzer
void ringBell() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}
