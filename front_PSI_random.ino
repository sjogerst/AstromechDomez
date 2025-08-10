#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

const int screenWidth = 320;
const int screenHeight = 240;

const int borderWidth = 30;
int borderX = screenWidth / 2;

const int backlightPin = 21;  // Typical CYD backlight pin; adjust if needed

unsigned long lastBorderChange = 0;
unsigned long borderChangeInterval = 1000;

unsigned long lastBrightnessChange = 0;
unsigned long brightnessChangeInterval = 5000;

void setup() {
  tft.init();
  tft.setRotation(1);  // Landscape orientation
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, 255);  // Start full brightness
  randomSeed(analogRead(0));
  
  borderChangeInterval = random(500, 5000);
  brightnessChangeInterval = random(3000, 10000);
  
  drawScreen();
}

void loop() {
  unsigned long now = millis();

  // Change border position randomly every 0.5 to 5 seconds
  if (now - lastBorderChange >= borderChangeInterval) {
    borderX = random(borderWidth / 2, screenWidth - borderWidth / 2);
    borderChangeInterval = random(500, 5000);
    lastBorderChange = now;
    drawScreen();
  }

  // Change brightness randomly every 3 to 10 seconds
  if (now - lastBrightnessChange >= brightnessChangeInterval) {
    int brightnessPercent = random(30, 101); // 30% to 100%
    int dutyCycle = map(brightnessPercent, 0, 100, 0, 255);
    analogWrite(backlightPin, dutyCycle);
    brightnessChangeInterval = random(3000, 10000);
    lastBrightnessChange = now;
  }
}

void drawScreen() {
  int halfBorder = borderWidth / 2;

  // Clamp borderX so it stays fully on screen
  if (borderX < halfBorder) borderX = halfBorder;
  if (borderX > screenWidth - halfBorder) borderX = screenWidth - halfBorder;

  // Define colors
  uint16_t skyBlue = tft.color565(135, 206, 235);
  uint16_t red = tft.color565(255, 0, 0);

  // Left side: solid sky blue
  int leftWidth = borderX - halfBorder;
  if (leftWidth > 0) {
    tft.fillRect(0, 0, leftWidth, screenHeight, skyBlue);
  }

  // Right side: solid red
  int rightX = borderX + halfBorder;
  int rightWidth = screenWidth - rightX;
  if (rightWidth > 0) {
    // Add +1 pixel to cover edge fully
    tft.fillRect(rightX, 0, rightWidth + 1, screenHeight, red);
  }

  // Border: gradient sky blue → purple → red
  for (int i = 0; i < borderWidth; i++) {
    float ratio = (float)i / (borderWidth - 1);
    uint8_t r, g, b;

    if (ratio < 0.5) {
      float subRatio = ratio / 0.5;
      r = (uint8_t)((1 - subRatio) * 135 + subRatio * 128);
      g = (uint8_t)((1 - subRatio) * 206 + subRatio * 0);
      b = (uint8_t)((1 - subRatio) * 235 + subRatio * 128);
    } else {
      float subRatio = (ratio - 0.5) / 0.5;
      r = (uint8_t)((1 - subRatio) * 128 + subRatio * 255);
      g = 0;
      b = (uint8_t)((1 - subRatio) * 128 + subRatio * 0);
    }

    uint16_t color = tft.color565(r, g, b);
    tft.drawFastVLine(borderX - halfBorder + i, 0, screenHeight, color);
  }
}
