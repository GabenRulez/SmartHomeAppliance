/*
This is a main file of our project for Embedded Systems course in AGH Kraków.
Additional code is in multiple .ino files in the same directory.
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* PROGRAM MODES */
enum programMode{
  modeOFF = 0,
  modeWarmLights = 1,
  modeRGBLights = 2
};

struct ProgramConfig {
  programMode currentMode;
  uint8_t warmLightsStrength;   // MAX = 100
  uint8_t RGBLightsRed;         // MAX = 255
  uint8_t RGBLightsGreen;       // MAX = 255
  uint8_t RGBLightsBlue;        // MAX = 255
};

#include "config.h"
#include "secretConfig.h"
