/* DEVICE */
void sendErrorToSerial(String message);
void initializeRandomSeed();
SemaphoreHandle_t initializeSemaphore();
QueueHandle_t initializeQueue(int queueLength, int sizeOfCell);
String stringIPAddress(IPAddress address);
void taskSleep(int miliseconds);
int normalizeBetween(int value, int from, int to);
int mapNumber(int value, int valueFrom, int valueTo, int resultFrom, int resultTo);


/* OLED SCREEN */
void initializeScreen();
void initializeDisplay();
void startScreenManagerTask();
void sendScreenControllerCommand(ScreenControllerCommand command);
void screenManagerTask(void *parameters);


/* ROTARY ENCODER */
void initializeRotaryEncoder();
void initializeRotaryEncoderInterrupt();
void rotaryEncoderInterrupt();
void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2);
void rotaryEncoderDebouncedHandler(void *pinBState);
void startRotaryEncoderButtonTask();
void rotaryEncoderButtonTask( void * parameters );
void sendRotaryEncoderInputCommand(RotaryEncoderInputCommand command);


/* WIFI SERVER + STATION */
void initializeAccessPoint();
void initializeAliveMessages();
void initializeMainPage();
void initializeModeSelectors();
void startServer();
void startStationTask();
void stationTask(void * parameters);
void startStation();
void initializeStationReconnect();
void stationReconnectEvent(WiFiEvent_t event, WiFiEventInfo_t info);


/* LED CONTROLLER */
void initializeLEDController();
void startLEDControllerTask();
void sendLEDControllerCommand(LEDControllerCommand command);
void LEDControllerTask(void *parameters);
void turnOffLights();
void turnOffLEDWorker();
void handleTwoColors(void *parameters);
