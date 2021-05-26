void setup() {
  Serial.begin(115200);
  initializeRandomSeed();

  initializeScreen();
  initializeDisplay();

  initializeRotaryEncoder();
  initializeRotaryEncoderInterrupt();
  startRotaryEncoderButtonTask();

  startScreenManagerTask();

  initializeAccessPoint();
  initializeAliveMessages();
  initializeMainPage();
  initializeModeSelectors();

  startServer();
  startStationTask();

  initializeLEDController();
  startLEDControllerTask();
}

void loop() {
}
