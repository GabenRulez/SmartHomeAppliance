/*
This is a main file of our project for Embedded Systems course in AGH Kraków.

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*  OLED SCREEN   */
#define Display_SCL_Pin 22       // IO22
#define Display_SDA_Pin 21       // IO21
#define SCREEN_WIDTH 128          // OLED display width, in pixels
#define SCREEN_HEIGHT 32          // OLED display height, in pixels
#define OLED_RESET     4          // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C       ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*  ROTARY ENCODER  */
#define BUTTON_PIN 19
#define LEFT_PIN 18
#define RIGHT_PIN 5


void initializeScreen();
void initializeDisplay();
void initializeRotaryEncoder();

SemaphoreHandle_t mutex_v;

  
void setup() {
  Serial.begin(115200);
  initializeScreen();
  initializeDisplay();
  initializeRotaryEncoder();

  /*display.cp437(true);*/

  //attachInterrupt(LEFT_PIN, interruptFunction, RISING);
  mutex_v = xSemaphoreCreateMutex();
  attachInterrupt(LEFT_PIN, ehe, RISING);
  //attachInterrupt(RIGHT_PIN, b_interrupt, CHANGE);
}

int result = 0;
int lastMillis = 0;


TaskHandle_t taskHandle = NULL;
int debounceTime = 30;

void ehe(){
  int pin_right = digitalRead(RIGHT_PIN);
  //Serial.println(pin_right);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t xInterfaceToService = NULL;

  xTimerPendFunctionCallFromISR(
    debounceInterrupt,
    (void*)pin_right,
    //NULL,
    ( uint32_t) xInterfaceToService,
    &xHigherPriorityTaskWoken  
  );
  portYIELD_FROM_ISR();
}

void debounceInterrupt(void *pvParameter1, uint32_t ulParameter2) {
  BaseType_t xInterfaceToService = (BaseType_t) ulParameter2;
  
  if(taskHandle != NULL){
    vTaskDelete(taskHandle);
  }
  int right_pin_value = (int) pvParameter1;
  xTaskCreatePinnedToCore(
    betterInterrupt,
    "debouncedTask",
    1000,
    (void*) right_pin_value,
    1,
    &taskHandle,
    0
  );
}

void betterInterrupt(void *gay){
  vTaskDelay(debounceTime);
  Serial.println((int)gay);
  if((int)gay == LOW) result++;
  else result--;
  taskHandle = NULL;
  vTaskDelete(NULL);
}

void loop() {
  //for(;;);
  //Serial.println("Button:" + String(digitalRead(BUTTON_PIN)) + "\tLeft:" + String(digitalRead(LEFT_PIN)) + "\tRight:" + String(digitalRead(RIGHT_PIN)) + "\tCzas:" + String(millis()));
  display.clearDisplay();
  display.setCursor(0,0);

  //xSemaphoreTake(mutex_v, 1000);
  int myResult = result;
  //xSemaphoreGive(mutex_v);
  //if(random(10) < 3) debounceInterrupt();
  display.println(myResult);
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
  display.println("Projekt z Systemow Wbudowanych");
  display.display();
}

void initializeRotaryEncoder(){
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LEFT_PIN, INPUT_PULLDOWN);
  pinMode(RIGHT_PIN, INPUT_PULLDOWN);
}
