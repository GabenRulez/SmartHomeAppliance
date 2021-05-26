Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initializeScreen() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) sendErrorToSerial("SSD1306 allocation failed");
  displayCore = xPortGetCoreID();
}


void initializeDisplay() {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setTextWrap(1);
  display.clearDisplay();
  screenControllerQueue = initializeQueue(SCREEN_QUEUE_LENGTH, sizeof( struct ScreenControllerCommand ) );
  /*display.cp437(true);*/
}


void startScreenManagerTask() {
  xTaskCreatePinnedToCore( screenManagerTask, "screenManagerTask", 3000, (void*) NULL, 2, NULL, displayCore );
}

void sendScreenControllerCommand(ScreenControllerCommand command) {
  xQueueSend( screenControllerQueue, &command, ( TickType_t ) 0 );
}

void displayWelcomeFrame() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("# Embedded  Systems #\n# ---- Project ---- #");
  display.display();
}

void displayDefaultFrame(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.drawFastHLine(0,0,10, WHITE);
  display.drawFastVLine(0,0,10, WHITE);
  display.display();
}

void displayWiFiStatus(){
  display.clearDisplay();
}


void screenManagerTask(void *parameters) {
  portTickType screenUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / FPS;
  ProgramConfig localProgramConfig;


  RotaryEncoderInputCommand rotaryEncoderInputCommand;
  ScreenControllerCommand screenControllerCommand;
  uint8_t counter = 0;

  while (true) {
    screenUpdateTick = xTaskGetTickCount();

    
    if ( xQueueReceive( rotaryEncoderInputQueue, &rotaryEncoderInputCommand, (TickType_t) 0)) {
        Serial.println("Got rotaryEncoderInputCommand: " + String(rotaryEncoderInputCommand.type));
        display.clearDisplay();
        display.setCursor(0,0);
        counter++;
        switch(rotaryEncoderInputCommand.type){
          case buttonPress:
            display.println("ButtonPressed");
            break;
          case left:
            display.println("Left");
            break;
          case right:
            display.println("Right");
            break;
        }
        display.print("Counter: " + String(counter));
        display.display();
    };

    if ( xQueueReceive( screenControllerQueue, &screenControllerCommand, (TickType_t) 0)) {
        Serial.println("Got screenControllerCommand: " + String(screenControllerCommand.type));
        display.clearDisplay();
        display.setCursor(0,0);
        switch(screenControllerCommand.type){
          case mainMenu:
            display.println("mainMenu");
            break;
          case warmLights:
            display.println("warmLights");
            break;
          case staticRGBColor:
            display.println("staticRGBColor");
            break;
          case twoColorsRGB:
            display.println("twoColorsRGB");
            break;
          case rainbowMode:
            display.println("rainbowMode");
            break;
          case showIPAddress:
            display.println("showIPAddress");
            break;
        }
        display.display();
    };

    
    vTaskDelayUntil( &screenUpdateTick, frameTime);
  }






  

  while (true) {
    

    
    if (xSemaphoreTake(programConfigSemaphore, (TickType_t) frameTime) == pdTRUE) {
      localProgramConfig = programConfig;
      xSemaphoreGive(programConfigSemaphore);
    }


    if (localProgramConfig.currentMode == modeOFF) {

    } else if (localProgramConfig.currentMode == modeWarmLights) {
      display.clearDisplay();
      display.setCursor(2, 2);
      display.println("modeWarmLights");
      display.print("strength: ");
      display.print(localProgramConfig.warmLightsStrength);
      display.display();

    } else if (localProgramConfig.currentMode == modeRGBLights) {
      display.clearDisplay();
      display.setCursor(2, 2);
      display.println("modeRGBLights");
      display.println("color (rgb): ");
      display.print("(");
      display.print(localProgramConfig.RGBLightsRed);
      display.print(", ");
      display.print(localProgramConfig.RGBLightsGreen);
      display.print(", ");
      display.print(localProgramConfig.RGBLightsBlue);
      display.print(")");
      display.display();
    } else {
      //display.clearDisplay();
      //display.display();
      displayDefaultFrame();
    }

    vTaskDelayUntil( &screenUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}
