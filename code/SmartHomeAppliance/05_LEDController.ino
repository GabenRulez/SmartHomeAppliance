Adafruit_NeoPixel stripRGB(RGB_STRIP_PIXELS, RGB_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void initializeLEDController() {
  // RGB
  stripRGB.begin();
  for (uint8_t j = 0; j < RGB_STRIP_PIXELS; j++) {
    stripRGB.setPixelColor(j, 0, 0, 0);
  }
  stripRGB.show();
  
  // WHITE
  ledcSetup(WHITE_STRIP_CHANNEL, WHITE_STRIP_PWM_FREQUENCY, WHITE_STRIP_PWM_RESOLUTION);
  ledcAttachPin(WHITE_STRIP_PIN, WHITE_STRIP_CHANNEL);
  
  // FreeRTOS
  LEDControllerQueue = initializeQueue(LED_QUEUE_LENGTH, sizeof( struct LEDControllerCommand ) );
  LEDWorkerTaskSemaphore = initializeSemaphore();
}

void startLEDControllerTask() {
  xTaskCreate( LEDControllerTask, "LEDControllerTask", 1000, (void*) NULL, 3, NULL );
}

void sendLEDControllerCommand(LEDControllerCommand command) {
  xQueueSend( LEDControllerQueue, &command, ( TickType_t ) 0 );
}


void LEDControllerTask(void *parameters) {
  portTickType updateTick = xTaskGetTickCount();
  struct LEDControllerCommand lastCommand = {staticColor};
  struct LEDControllerCommand command;
  LEDWorkerTaskHandle = NULL;

  while (true) {
    if ( xQueueReceive( LEDControllerQueue, &command, (TickType_t) 100)) {
      switch (command.type) {

        case lightsOFF:
          if (lastCommand.type != lightsOFF) {
            turnOffLights();
          }
          break;

        case warmLightsON:
          if (lastCommand.type != lightsOFF && lastCommand.type != warmLightsON) {
            turnOffLights();
          } 
          for (uint8_t i = ANIMATION_LENGTH; i > 0; i--) {
            uint8_t brightness = ( lastCommand.brightness * i + command.brightness * (ANIMATION_LENGTH - i) ) / ANIMATION_LENGTH;
            ledcWrite(WHITE_STRIP_CHANNEL, brightness);
            taskSleep(checkTime);
          }
          break;

        case staticColor:
          uint8_t oldRed, oldGreen, oldBlue;
          switch (lastCommand.type) {
            case staticColor:
              oldRed = lastCommand.red1;
              oldGreen = lastCommand.green1;
              oldBlue = lastCommand.blue1;
              break;

            default:
              turnOffLights();
            case lightsOFF:
              oldRed = 0;
              oldGreen = 0;
              oldBlue = 0;
              break;
          }
          for (uint8_t i = ANIMATION_LENGTH; i > 0; i--) {
            uint8_t newRed, newGreen, newBlue;
            uint8_t newPart = ANIMATION_LENGTH - i + 1, oldPart = i - 1;
            newRed    = ( command.red1   * newPart +  oldRed    * oldPart ) / ANIMATION_LENGTH;
            newGreen  = ( command.green1 * newPart +  oldGreen  * oldPart ) / ANIMATION_LENGTH;
            newBlue   = ( command.blue1  * newPart +  oldBlue   * oldPart ) / ANIMATION_LENGTH;
            for (uint8_t j = 0; j < RGB_STRIP_PIXELS; j++) {
              stripRGB.setPixelColor(j, newRed, newGreen, newBlue);
            }
            stripRGB.show();
            taskSleep(checkTime);
          }
          break;

        case twoColors:
          if (lastCommand.type != lightsOFF) {
            turnOffLights();
          } else turnOffLEDWorker();
          globalLEDCommand = command;
          xTaskCreate( handleTwoColors, "handleTwoColors", 2000, (void*) NULL, 3, &LEDWorkerTaskHandle);
          break;

        case threeColors:
          break;

        case rainbow:
          if (lastCommand.type != lightsOFF) {
            turnOffLights();
          };
          globalLEDCommand = command;
          xTaskCreate( handleRainbow, "handleRainbow", 2000, (void*) NULL, 3, &LEDWorkerTaskHandle);
          break;
      }
      lastCommand = command;
    }
    vTaskDelayUntil( &updateTick, checkTime);
  }
  vTaskDelete(NULL);
}

void turnOffLights() {
  uint8_t whiteLEDsBrightness = ledcRead(WHITE_STRIP_CHANNEL);
  
  for (uint8_t i = ANIMATION_LENGTH; i > 0; i--) {
    stripRGB.setBrightness( (int) RGBCurrentBrightness * i / ANIMATION_LENGTH );
    stripRGB.show();
    ledcWrite(WHITE_STRIP_CHANNEL, whiteLEDsBrightness * (i - 1) / ANIMATION_LENGTH);
    taskSleep(checkTime);
  }
  turnOffLEDWorker();
  for (uint8_t i = 0; i < RGB_STRIP_PIXELS; i++) {
    stripRGB.setPixelColor(i, 0, 0, 0);
  }
  stripRGB.show();
  stripRGB.setBrightness(RGBDefaultBrightness);
  RGBCurrentBrightness = RGBDefaultBrightness;
}


void turnOffLEDWorker() {
  if (xSemaphoreTake(LEDWorkerTaskSemaphore, (TickType_t) 1000) == pdTRUE) {
    if (LEDWorkerTaskHandle != NULL) {
      vTaskDelete(LEDWorkerTaskHandle);
      LEDWorkerTaskHandle = NULL;
    }
    xSemaphoreGive(LEDWorkerTaskSemaphore);
  }
}




void handleTwoColors(void *parameters) {
  struct LEDControllerCommand command = globalLEDCommand;
  float animationIntervalMultiplier = 1.0 * command.animationIntervalMultiplier / 256;
  uint16_t moddedAnimationLength = floor(32.0 * ANIMATION_LENGTH * animationIntervalMultiplier);
  uint16_t j = 0;
  uint8_t iter = 0;

  portTickType updateTick = xTaskGetTickCount();
  while (true) {
    if(iter < ANIMATION_LENGTH){
      iter++;
      stripRGB.setBrightness( (int) RGBDefaultBrightness * iter / ANIMATION_LENGTH );
    };
    j = (j+1) % (moddedAnimationLength);
    updateTick = xTaskGetTickCount();
    if (xSemaphoreTake(LEDWorkerTaskSemaphore, (TickType_t) 10) == pdTRUE){

      float parameter_1 = 0.5 + sin(TWO_PI * (float)j  / (moddedAnimationLength)) / 2.0;
      float parameter_2 = 1.0 - parameter_1;
  
      uint8_t newRed, newGreen, newBlue;
      newRed    = floor(parameter_1 * command.red1    + parameter_2 * command.red2    );
      newGreen  = floor(parameter_1 * command.green1  + parameter_2 * command.green2  );
      newBlue   = floor(parameter_1 * command.blue1   + parameter_2 * command.blue2   );
  
      for (uint8_t i = 0; i < RGB_STRIP_PIXELS; i++) {
        stripRGB.setPixelColor(i, newRed, newGreen, newBlue);
      }
      stripRGB.show();

      xSemaphoreGive(LEDWorkerTaskSemaphore);
    }
    vTaskDelayUntil( &updateTick, checkTime);
  }
  vTaskDelete(NULL);
}

void handleRainbow(void *parameters) {
  struct LEDControllerCommand command = globalLEDCommand;
  float animationIntervalMultiplier = 1.0 * command.animationIntervalMultiplier / 256;
  uint16_t moddedAnimationLength = floor(32.0 * ANIMATION_LENGTH * animationIntervalMultiplier);
  uint16_t j = 0;
  uint8_t iter = 0;

  portTickType updateTick = xTaskGetTickCount();
  while (true) {
    if(iter < ANIMATION_LENGTH){
      iter++;
      int brightness = (int( command.brightness * iter / ANIMATION_LENGTH));
      stripRGB.setBrightness( brightness );
      RGBCurrentBrightness = brightness;
    };
    j = (j+1) % (moddedAnimationLength);
    updateTick = xTaskGetTickCount();
    if (xSemaphoreTake(LEDWorkerTaskSemaphore, (TickType_t) 10) == pdTRUE){

    int hue = mapNumber(j, 0, moddedAnimationLength - 1, 0, 65535);
      for (uint8_t i = 0; i < RGB_STRIP_PIXELS; i++) {
        int tempHue = (hue + 65536 * i / RGB_STRIP_PIXELS)%65536;
        stripRGB.setPixelColor(i, stripRGB.gamma32(stripRGB.ColorHSV(tempHue)));
      }
      stripRGB.show();

      xSemaphoreGive(LEDWorkerTaskSemaphore);
    }
    vTaskDelayUntil( &updateTick, checkTime);
  }
  vTaskDelete(NULL);
}
