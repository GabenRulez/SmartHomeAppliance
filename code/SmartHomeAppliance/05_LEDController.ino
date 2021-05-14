void initializeLEDController() {
  stripRGB.begin();
  for (uint8_t j = 0; j < NEOPIXEL_PIXELS; j++) {
    stripRGB.setPixelColor(j, 0, 0, 0);
  }
  stripRGB.show();
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
      Serial.println(command.type);
      switch (command.type) {

        case lightsOFF:
          if (lastCommand.type != lightsOFF) {
            turnOffLights();
          }
          break;

        case warmLightsON:
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
            for (uint8_t j = 0; j < NEOPIXEL_PIXELS; j++) {
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
          break;
      }
      lastCommand = command;
    }
    vTaskDelayUntil( &updateTick, checkTime);
  }
  vTaskDelete(NULL);
}

void turnOffLights() {
  for (uint8_t i = ANIMATION_LENGTH; i > 0; i--) {
    stripRGB.setBrightness( (int) RGBBrightness * i / ANIMATION_LENGTH );
    stripRGB.show();
    taskSleep(checkTime);
  }
  turnOffLEDWorker();
  for (uint8_t i = 0; i < NEOPIXEL_PIXELS; i++) {
    stripRGB.setPixelColor(i, 0, 0, 0);
  }
  stripRGB.show();
  stripRGB.setBrightness(RGBBrightness);
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
  uint16_t j = 0;

  portTickType updateTick = xTaskGetTickCount();
  while (true) {
    j = (j+1) % (32 * ANIMATION_LENGTH);
    updateTick = xTaskGetTickCount();
    if (xSemaphoreTake(LEDWorkerTaskSemaphore, (TickType_t) 10) == pdTRUE){

      float parameter_1 = 0.5 + sin(TWO_PI * (float)j / (32.0 * ANIMATION_LENGTH)) / 2.0;
      float parameter_2 = 1.0 - parameter_1;
  
      uint8_t newRed, newGreen, newBlue;
      newRed    = floor(parameter_1 * command.red1    + parameter_2 * command.red2    );
      newGreen  = floor(parameter_1 * command.green1  + parameter_2 * command.green2  );
      newBlue   = floor(parameter_1 * command.blue1   + parameter_2 * command.blue2   );
  
      for (uint8_t i = 0; i < NEOPIXEL_PIXELS; i++) {
        stripRGB.setPixelColor(i, newRed, newGreen, newBlue);
      }
      stripRGB.show();

      xSemaphoreGive(LEDWorkerTaskSemaphore);
    }
    vTaskDelayUntil( &updateTick, checkTime);
  }
  vTaskDelete(NULL);
}
