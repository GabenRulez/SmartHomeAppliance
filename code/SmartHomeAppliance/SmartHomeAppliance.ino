/*
  This is a main file of our project for Embedded Systems course in AGH Kraków.
  Additional code is in multiple .ino files in the same directory.
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* PROGRAM MODES */
enum programMode {
  modeOFF = 0,
  modeWarmLights = 1,
  modeRGBLights = 2,
  showIP = 99
};

struct ProgramConfig {
  programMode currentMode;
  uint8_t warmLightsStrength;   // MAX = 100
  uint8_t RGBLightsRed;         // MAX = 255
  uint8_t RGBLightsGreen;       // MAX = 255
  uint8_t RGBLightsBlue;        // MAX = 255
};

enum LEDControllerCommandType {
  lightsOFF = 0,
  warmLightsON = 1,
  staticColor = 2,
  twoColors = 3,
  threeColors = 4,
  rainbow = 5
};

struct LEDControllerCommand {
  LEDControllerCommandType type;
  uint8_t brightness; 
  uint8_t animationIntervalMultiplier;
  uint8_t red1;
  uint8_t green1;
  uint8_t blue1;
  uint8_t red2;
  uint8_t green2;
  uint8_t blue2;
  uint8_t red3;
  uint8_t green3;
  uint8_t blue3;
};


#include "config.h"
#include "secretConfig.h"
