Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initializeScreen(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) sendErrorToSerial("SSD1306 allocation failed");
  displayCore = xPortGetCoreID();
}


void initializeDisplay(){
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextWrap(5);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("# Embedded  Systems #\n# ---- Project ---- #");
  display.display();
  delay(2000);
  /*display.cp437(true);*/
}


void startScreenManagerTask(){
  xTaskCreatePinnedToCore( screenManagerTask, "screenManagerTask", 1000, (void*) NULL, 2, NULL, displayCore );
}


void screenManagerTask(void *parameters){
  portTickType screenUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / FPS;
  ProgramConfig localProgramConfig;
  
  while(true){
    if (xSemaphoreTake(programConfigSemaphore, (TickType_t) frameTime) == pdTRUE){
      localProgramConfig = programConfig;
      xSemaphoreGive(programConfigSemaphore);
    }
    
    
    if(localProgramConfig.currentMode == modeOFF){
      if (xSemaphoreTake(rotaryEncoderSemaphore, (TickType_t) 1000) == pdTRUE){
        int gotValue = rotaryEncoderValue;
        boolean buttonPressed = rotaryEncoderButtonPressed;
        xSemaphoreGive(rotaryEncoderSemaphore);
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(gotValue);
        if(buttonPressed) display.fillCircle(display.width()/2, display.height()/2, 4, WHITE);
        display.display();
      }
      
    } else if (localProgramConfig.currentMode == modeWarmLights){
      display.clearDisplay();
      display.setCursor(2,2);
      display.println("modeWarmLights");
      display.print("strength: "); 
      display.print(localProgramConfig.warmLightsStrength);
      display.display();
      
    } else if (localProgramConfig.currentMode == modeRGBLights){
      display.clearDisplay();
      display.setCursor(2,2);
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
    } else{
      display.clearDisplay();
      display.display();
    }
    
    vTaskDelayUntil( &screenUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}
