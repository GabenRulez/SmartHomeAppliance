/*
This is a main file of our project for Embedded Systems course in AGH Kraków.

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* DEVICE */
#define AVAILABLE_CORES   ESP.getChipCores()
#define DISCONNECTED_PIN  32


/*  OLED SCREEN   */
#define Display_SCL_Pin   22       // IO22
#define Display_SDA_Pin   21       // IO21
#define SCREEN_WIDTH      128      // OLED display width, in pixels
#define SCREEN_HEIGHT     32       // OLED display height, in pixels
#define OLED_RESET        4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS    0x3C     //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define WHITE             SSD1306_WHITE
#define BLACK             SSD1306_BLACK
#define FPS               60
int displayCore         = 0;       // Overwritten later. It's the core, on which the I^2C communication will take place on.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*  ROTARY ENCODER  */
#define ROTARY_ENCODER_PIN_A                18
#define ROTARY_ENCODER_PIN_B                5
#define ROTARY_ENCODER_BUTTON_PIN           19
#define ROTARY_ENCODER_BUTTON_POLLING_RATE  60
TaskHandle_t      rotaryEncoderInterruptTaskHandle  = NULL;
const int         rotaryEncoderDebounceTime         = 33;
int               rotaryEncoderValue                = 0;
boolean           rotaryEncoderButtonPressed        = false;
SemaphoreHandle_t rotaryEncoderSemaphore            = NULL;



void sendErrorToSerial(String message);
void initializeScreen();
void initializeDisplay();
void initializeRandomSeed();
void initializeRotaryEncoder();
void initializeRotaryEncoderInterrupt();
void rotaryEncoderInterrupt();
void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2);
void rotaryEncoderDebouncedHandler(void *pinBState);
void rotaryEncoderButtonInterrupt( void * parameters );
void screenManagerTask(void *parameters);
void initializeScreenManagerTask();
SemaphoreHandle_t initializeSemaphore();

void setup() {
  Serial.begin(115200);
  initializeScreen();
  initializeDisplay();
  initializeRandomSeed();
  initializeRotaryEncoder();
  initializeRotaryEncoderInterrupt();
  initializeScreenManagerTask();

  xTaskCreatePinnedToCore( rotaryEncoderButtonInterrupt, "rotaryEncoderButtonInterrupt", 3000, (void*) NULL, 1, NULL,0 );
}


// dim(true/false) - przyciemnienie lekkie ekranu (spoko)
// display.setTextSize() - defaultowa czcionka ma rozmiar 5x8, czyli rozmiar 4 jest maximum (na wysokość)
// display.setTextWrap(w) - zmienia szerokosc "otoczki" dookola literek


void loop() {
}





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


void initializeRandomSeed(){
  randomSeed(analogRead(DISCONNECTED_PIN));
}


void initializeRotaryEncoder(){
  pinMode(ROTARY_ENCODER_BUTTON_PIN,  INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_A,       INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_B,       INPUT_PULLDOWN);
  rotaryEncoderSemaphore = initializeSemaphore();
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

  portMUX_TYPE mutexStop = portMUX_INITIALIZER_UNLOCKED;
  taskENTER_CRITICAL(&mutexStop); // It's here to stop this task from being deleted, while the other semaphore is taken.

  if (xSemaphoreTake(rotaryEncoderSemaphore, (TickType_t) 1000) == pdTRUE){
    if((int)pinBState == LOW) rotaryEncoderValue++;
    else rotaryEncoderValue--;
    xSemaphoreGive(rotaryEncoderSemaphore);
   }

  taskEXIT_CRITICAL(&mutexStop);
  
  rotaryEncoderInterruptTaskHandle = NULL;
  vTaskDelete(NULL);
}


void rotaryEncoderButtonInterrupt( void * parameters ){

  portTickType pollingUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / ROTARY_ENCODER_BUTTON_POLLING_RATE;
  boolean oldState = false;
  boolean newState;
  
  if (xSemaphoreTake(rotaryEncoderSemaphore, (TickType_t) 1000) == pdTRUE){
    oldState = rotaryEncoderButtonPressed;
    xSemaphoreGive(rotaryEncoderSemaphore);
  }
  
  while(true){
    newState = (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == HIGH);
    if(newState == HIGH && oldState == false){
      vTaskDelay(frameTime/2);
      newState = (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == HIGH);
    }
    if(newState != oldState){
      if (xSemaphoreTake(rotaryEncoderSemaphore, (TickType_t) 1000) == pdTRUE){
        rotaryEncoderButtonPressed = newState;
        xSemaphoreGive(rotaryEncoderSemaphore);
        oldState = newState;
      }
    }
    vTaskDelayUntil( &pollingUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}


void initializeScreenManagerTask(){
  xTaskCreatePinnedToCore( screenManagerTask, "screenManagerTask", 1000, (void*) NULL, 2, NULL, displayCore );
}


void screenManagerTask(void *parameters){
  portTickType screenUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / FPS;

  while(true){
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
    vTaskDelayUntil( &screenUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}


SemaphoreHandle_t initializeSemaphore(){
  SemaphoreHandle_t tempSemaphore = xSemaphoreCreateBinary();
  if( tempSemaphore != NULL) xSemaphoreGive(tempSemaphore);
  else sendErrorToSerial("Failed initializing semaphore!");
  return tempSemaphore;
}


void sendErrorToSerial(String message){
  for(;;){
    Serial.println("Error: " + message);
    delay(100); 
  }
}
