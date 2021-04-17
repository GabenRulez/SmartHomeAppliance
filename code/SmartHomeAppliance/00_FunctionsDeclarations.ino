/* DEVICE */
void sendErrorToSerial(String message);
void initializeRandomSeed();
SemaphoreHandle_t initializeSemaphore();


/* OLED SCREEN */
void initializeScreen();
void initializeDisplay();
void startScreenManagerTask();
void screenManagerTask(void *parameters);


/* ROTARY ENCODER */
void initializeRotaryEncoder();
void initializeRotaryEncoderInterrupt();
void rotaryEncoderInterrupt();
void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2);
void rotaryEncoderDebouncedHandler(void *pinBState);
void startRotaryEncoderButtonTask();
void rotaryEncoderButtonTask( void * parameters );
