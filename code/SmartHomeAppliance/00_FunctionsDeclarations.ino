/* DEVICE */
void sendErrorToSerial(String message);
void initializeRandomSeed();
SemaphoreHandle_t initializeSemaphore();
String stringIPAddress(IPAddress address);


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


/* WIFI SERVER + STATION */
void initializeAccessPoint();
void initializeAliveMessages();
void initializeMainPage();
void startServer();
void startStation();
void stationReconnectEvent(WiFiEvent_t event, WiFiEventInfo_t info);
void initializeStationReconnect();



/* PROGRAM MODES */
enum programMode{
  modeOFF = 0,
  modeWarmLights = 1,
  modeRGB = 2
};
