AsyncWebServer server(SERVER_PORT);

void initializeAccessPoint() {
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  AccessPointIPAddress = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(AccessPointIPAddress);
}


void initializeAliveMessages() {
  server.on("/alive", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "alive");
  });
}


void initializeMainPage() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println(request->url() + " -- " + request->host());
    request->send(200, "text/html", "<!DOCTYPE HTML><body>hejj</body>");
  });
}


void initializeModeSelectors() {
  server.on("/warmLights", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("strength")){
      uint8_t requestedStrength = normalizeBetween( request->getParam("strength")->value().toInt(), 0, 255);
      
      if (xSemaphoreTake(programConfigSemaphore, (TickType_t) 1000) == pdTRUE) {
        programConfig.currentMode = modeWarmLights;
        programConfig.warmLightsStrength = requestedStrength;
        xSemaphoreGive(programConfigSemaphore);
        request->send(200, "text/plain", "updated");
      }

      LEDControllerCommand command = {warmLightsON, requestedStrength};
      sendLEDControllerCommand(command);
      
    }
    request->send(403, "text/plain", "No 'strength' parameter");
  });


  server.on("/RGBLights", HTTP_GET, [](AsyncWebServerRequest * request) {
    int red, blue, green;
    if (request->hasParam("red") && request->hasParam("green") && request->hasParam("blue")) {
      red     = normalizeBetween( request->getParam("red")->value().toInt(),    0, 255);
      green   = normalizeBetween( request->getParam("green")->value().toInt(),  0, 255);
      blue    = normalizeBetween( request->getParam("blue")->value().toInt(),   0, 255);

      LEDControllerCommand command = {staticColor, 0, 0, red, green, blue};
      sendLEDControllerCommand(command);

      if (xSemaphoreTake(programConfigSemaphore, (TickType_t) 1000) == pdTRUE) {

        programConfig.currentMode = modeRGBLights;
        programConfig.RGBLightsRed = red;
        programConfig.RGBLightsGreen = green;
        programConfig.RGBLightsBlue = blue;

        xSemaphoreGive(programConfigSemaphore);
        request->send(200, "text/plain", "updated");
      }
    }
    request->send(403, "text/plain", "No 'red', 'green', 'blue' parameters");
  });


  server.on("/twoColors", HTTP_GET, [](AsyncWebServerRequest * request) {
    int red1, green1, blue1, red2, green2, blue2, animationIntervalMultiplier;
    if (request->hasParam("red1") && request->hasParam("green1") && request->hasParam("blue1") && request->hasParam("red2") && request->hasParam("green2") && request->hasParam("blue2") && request->hasParam("speed")){
      red1    = normalizeBetween(  request->getParam("red1")->value().toInt()    , 0, 255);
      green1  = normalizeBetween(  request->getParam("green1")->value().toInt()  , 0, 255);
      blue1   = normalizeBetween(  request->getParam("blue1")->value().toInt()   , 0, 255);
      red2    = normalizeBetween(  request->getParam("red2")->value().toInt()    , 0, 255);
      green2  = normalizeBetween(  request->getParam("green2")->value().toInt()  , 0, 255);
      blue2   = normalizeBetween(  request->getParam("blue2")->value().toInt()   , 0, 255);
      animationIntervalMultiplier = normalizeBetween(  request->getParam("speed")->value().toInt() , 1, 255);
      
      request->send(200, "text/plain", "updated");
      LEDControllerCommand command = {twoColors, 0, animationIntervalMultiplier, red1, green1, blue1, red2, green2, blue2};
      sendLEDControllerCommand(command);
    }
    request->send(403, "text/plain", "No 'red1', 'green1', 'blue1', 'red2', 'green2', 'blue2' parameters");
  });


  server.on("/rainbow", HTTP_GET, [](AsyncWebServerRequest * request) {
    uint8_t strength, animationSpeed;
    if (request->hasParam("strength") && request->hasParam("speed")) {
      strength        = normalizeBetween( request->getParam("strength")->value().toInt(),   0, 255);
      animationSpeed  = normalizeBetween( request->getParam("speed")->value().toInt(),      0, 255);

      LEDControllerCommand command = {rainbow, strength, animationSpeed};
      sendLEDControllerCommand(command);

      request->send(200, "text/plain", "updated");
    }
    request->send(403, "text/plain", "No 'strength' and 'speed' parameters");
  });
 
  

  server.on("/modeOFF", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (xSemaphoreTake(programConfigSemaphore, (TickType_t) 1000) == pdTRUE) {
      programConfig.currentMode = modeOFF;
      xSemaphoreGive(programConfigSemaphore);
      request->send(200, "text/plain", "updated");

      LEDControllerCommand command = {lightsOFF};
      sendLEDControllerCommand(command);
    }
  });
}


void startServer() {
  // TODO delete line below when going into production
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.begin();
}




void startStationTask() {
  xTaskCreate( stationTask, "stationTask", 3000, (void*) NULL, 1, NULL);
}

void stationTask(void * parameters) {
  startStation();
  initializeStationReconnect();
  vTaskDelete(NULL);
}


void startStation() {
  WiFi.disconnect(true);
  WiFi.begin(ST_SSID, ST_PASSWORD);
  int reconnectionsLeft = RECONNECTION_MAX_COUNT;
  while (reconnectionsLeft > 0 && (WiFi.status() != WL_CONNECTED)) {
    Serial.println("Connecting to <" + String(ST_SSID) + "> Status: " + String(WiFi.status()));
    if (WiFi.status() == WL_DISCONNECTED) {
      WiFi.reconnect();
      vTaskDelay(RECONNECTION_TIMEOUT);
    }
    vTaskDelay(RECONNECTION_TIMEOUT);
    reconnectionsLeft--;
  }
  StationIPAddress = WiFi.localIP();
  if (WiFi.status() == WL_CONNECTED) Serial.println("Successfully connected to <" + String(ST_SSID) + ">. Local IP address: " + stringIPAddress(StationIPAddress));
  else Serial.println("Couldn't connect to <" + String(ST_SSID) + ">");
}


void initializeStationReconnect() {
  WiFi.onEvent(stationReconnectEvent, SYSTEM_EVENT_STA_DISCONNECTED);
}


void stationReconnectEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  startStation();
}
