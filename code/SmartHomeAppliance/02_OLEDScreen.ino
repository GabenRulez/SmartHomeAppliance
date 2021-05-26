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

void displayDefaultFrame(float multiplier){
  uint8_t lineLength = floor(multiplier * min(SCREEN_WIDTH, SCREEN_HEIGHT)/8.0);
  
  display.drawFastHLine(0,                            0,                              lineLength, WHITE);
  display.drawFastVLine(0,                            0,                              lineLength, WHITE);

  display.drawFastHLine(SCREEN_WIDTH - lineLength,    0,                              lineLength, WHITE);
  display.drawFastVLine(SCREEN_WIDTH - 1,             0,                              lineLength, WHITE);

  display.drawFastHLine(0,                            SCREEN_HEIGHT - 1,              lineLength, WHITE);
  display.drawFastVLine(0,                            SCREEN_HEIGHT - lineLength,     lineLength, WHITE);

  display.drawFastHLine(SCREEN_WIDTH - lineLength,    SCREEN_HEIGHT - 1,              lineLength, WHITE);
  display.drawFastVLine(SCREEN_WIDTH - 1,             SCREEN_HEIGHT - lineLength,     lineLength, WHITE);
}


void screenManagerTask(void *parameters) {
  portTickType screenUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / FPS;
  
  RotaryEncoderInputCommand rotaryEncoderInputCommand;
  ScreenControllerCommand screenControllerCommand;
  ScreenControllerCommandType currentMode = mainMenu;

  uint8_t depth = 0;

  while (true) {
    screenUpdateTick = xTaskGetTickCount();

    
    while ( xQueueReceive( rotaryEncoderInputQueue, &rotaryEncoderInputCommand, (TickType_t) 0)) {
      switch(depth){
        case 0:
          ScreenControllerCommand screenControllerCommand;
          switch(rotaryEncoderInputCommand.type){
            case buttonPress:
              displayDefaultFrame(1.5);
              display.display();
              break;
              
            case left:
              screenControllerCommand = {(ScreenControllerCommandType)((currentMode + SCREEN_COMMAND_COUNT - 1) % SCREEN_COMMAND_COUNT)};
              sendScreenControllerCommand(screenControllerCommand);
              break;
              
            case right:
              screenControllerCommand = {(ScreenControllerCommandType)((currentMode + 1) % SCREEN_COMMAND_COUNT)};
              sendScreenControllerCommand(screenControllerCommand);
              break;
          }
          break;
          
        default:
          break;
      }
    };

    if ( xQueueReceive( screenControllerQueue, &screenControllerCommand, (TickType_t) 0)) {
        Serial.println("Got screenControllerCommand: " + String(screenControllerCommand.type)); // DEBUG
       
        display.setCursor(0,0);
        
        currentMode = screenControllerCommand.type;
        
        switch(currentMode){
          case mainMenu:
            display.clearDisplay();
            depth = 0;
            displayDefaultFrame(1);
            displayMainMenu();
            break;
            
          case warmLights:
            display.clearDisplay();
            depth = 0;    
            displayDefaultFrame(1);
            displayWarmLights();
            break;
            
          case staticRGBColor:
            display.clearDisplay();
            depth = 0;
            displayDefaultFrame(1);
            displayStaticRGB();
            break;
            
          case twoColorsRGB:
            display.clearDisplay();
            depth = 0;
            displayDefaultFrame(1);
            displayTransitionRGB();
            break;
            
          case rainbowMode:
            display.clearDisplay();
            depth = 0;
            displayDefaultFrame(1);
            displayRainbowMode();
            break;
            
          case showIPAddress:
            display.clearDisplay();
            depth = 0;
            displayDefaultFrame(1);
            displayIPAddress();
            break;
        }
        display.display();
    };

    
    vTaskDelayUntil( &screenUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}

void displayMainMenu(){
  display.setCursor(4, 8);
  display.setTextSize(2);
  display.print("Lights OFF");
  display.setTextSize(1);
}

void displayWarmLights(){
  display.setCursor(4, 8);
  display.setTextSize(2);
  display.print("Warm Light");
  display.setTextSize(1);
}

void displayStaticRGB(){
  display.setCursor(4, 8);
  display.setTextSize(2);
  display.print("Static RGB");      
  display.setTextSize(1);
}

void displayTransitionRGB(){
  display.setCursor(10, 0);
  display.setTextSize(2);
  display.print("Two-color");
  display.setCursor(46, 16);
  display.print("RGB");
  display.setTextSize(1);
}

void displayRainbowMode(){
  display.setCursor(22, 8);
  display.setTextSize(2);
  display.print("Rainbow");
  display.setTextSize(1);
}


void displayIPAddress(){
  display.setCursor(34, 0);
  display.print("IP Address");
  display.setCursor(2,8);
  display.println("ST: " + stringIPAddress(StationIPAddress));
  display.setCursor(2,16);
  display.println("AP: " + stringIPAddress(AccessPointIPAddress));
}


/*

      int16_t x1, y1;
    uint16_t w1, h1; 
  Serial.println("BOUNDS");
  display.getTextBounds("IP Address", 2, 2, &x1, &y1, &w1, &h1);
  Serial.println( String(w1) + " - " + String(h1));


 */
