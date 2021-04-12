/*
This is a main file of our project for Embedded Systems course in AGH Kraków.

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*  OLED SCREEN   */
#define Display_SCL_Pin   22       // IO22
#define Display_SDA_Pin   21       // IO21
#define SCREEN_WIDTH      128      // OLED display width, in pixels
#define SCREEN_HEIGHT     32       // OLED display height, in pixels
#define OLED_RESET        4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS    0x3C     //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*  ROTARY ENCODER  */
#define ROTARY_ENCODER_BUTTON_PIN   19
#define ROTARY_ENCODER_PIN_A        18
#define ROTARY_ENCODER_PIN_B        5
TaskHandle_t rotaryEncoderInterruptTaskHandle = NULL;
const int rotaryEncoderDebounceTime = 33;
int rotaryEncoderValue = 0;


void initializeScreen();
void initializeDisplay();
void initializeRotaryEncoder();
void initializeRotaryEncoderInterrupt();
void rotaryEncoderInterrupt();
void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2);
void rotaryEncoderDebouncedHandler(void *pinBState);
void initializeMutex();


void setup() {
  Serial.begin(115200);
  initializeScreen();
  initializeDisplay();
  initializeRotaryEncoder();
  initializeRotaryEncoderInterrupt();
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(rotaryEncoderValue);
  display.display();
  delay(10);
}

void initializeScreen(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void initializeDisplay(){
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("# Embedded  Systems #\n# ---- Project ---- #");
  display.display();
  delay(2000);
  /*display.cp437(true);*/
}

void initializeRotaryEncoder(){
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_A,   INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_B,  INPUT_PULLDOWN);
}

void initializeRotaryEncoderInterrupt(){
  attachInterrupt(ROTARY_ENCODER_PIN_A, rotaryEncoderInterrupt, RISING);
}

void rotaryEncoderInterrupt(){
  int pinBState = digitalRead(ROTARY_ENCODER_PIN_B);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t xInterfaceToService = NULL;
  
  xTimerPendFunctionCallFromISR( rotaryEncoderInterruptInISR, (void*)pinBState, ( uint32_t) xInterfaceToService, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR();
}

void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2) {
  BaseType_t xInterfaceToService = (BaseType_t) ulParameter2;
  
  if(rotaryEncoderInterruptTaskHandle != NULL){ vTaskDelete(rotaryEncoderInterruptTaskHandle); }
  
  int pinBState = (int) pvParameter1;
  xTaskCreatePinnedToCore( rotaryEncoderDebouncedHandler, "debouncedTask", 1000, (void*) pinBState, 1, &rotaryEncoderInterruptTaskHandle, 0 );
}

void rotaryEncoderDebouncedHandler(void *pinBState){
  vTaskDelay(rotaryEncoderDebounceTime);
  
  if((int)pinBState == LOW) rotaryEncoderValue++;
  else rotaryEncoderValue--;
  
  rotaryEncoderInterruptTaskHandle = NULL;
  vTaskDelete(NULL);
}

void initializeMutex(){
  //SemaphoreHandle_t mutex_v;
  //mutex_v = xSemaphoreCreateMutex();
  //xSemaphoreTake(mutex_v, 1000);
  //xSemaphoreGive(mutex_v);
}
