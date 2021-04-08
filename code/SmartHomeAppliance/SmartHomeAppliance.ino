/*
This is a main file of our project for Embedded Systems course in AGH Kraków.

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*  OLED SCREEN   */
#define Display_SCL_Pin 22;       // IO22
#define Display_SDA_Pin 21;       // IO21
#define SCREEN_WIDTH 128          // OLED display width, in pixels
#define SCREEN_HEIGHT 32          // OLED display height, in pixels
#define OLED_RESET     4          // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C       ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initializeScreen();

void setup() {
  Serial.begin(9600);
  initializeScreen();
  display.display();
  display.clearDisplay();
  display.drawRect(2,2, 30, 30, SSD1306_WHITE);
  display.display();

}

int global_i = 0;
void loop() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.cp437(true);
    display.write(global_i);
    display.display();
    delay(100);
 
  global_i = (global_i + 1 ) % 256;
}

void initializeScreen(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}
