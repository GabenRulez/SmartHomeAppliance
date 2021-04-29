AsyncWebServer server(SERVER_PORT);

void initializeAccessPoint(){
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  AccessPointIPAddress = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(AccessPointIPAddress);
}


void initializeAliveMessages(){
  server.on("/alive", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "alive");
  });
}


void initializeMainPage(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(request->url() + " -- " + request->host());
    request->send(200, "text/html", "<!DOCTYPE HTML><body>hejj</body>");
  });
}


void initializeModeSelectors(){
  server.on("/warmLights", HTTP_GET, [](AsyncWebServerRequest *request){
    if (xSemaphoreTake(programModeSemaphore, (TickType_t) 1000) == pdTRUE){
      currentProgramMode = modeWarmLights;
      xSemaphoreGive(programModeSemaphore);
      request->send(200, "text/plain", "updated");
    }
  });

  server.on("/modeOFF", HTTP_GET, [](AsyncWebServerRequest *request){
    if (xSemaphoreTake(programModeSemaphore, (TickType_t) 1000) == pdTRUE){
      currentProgramMode = modeOFF;
      xSemaphoreGive(programModeSemaphore);
      request->send(200, "text/plain", "updated");
    }
  });
}


void startServer(){
  // TODO delete line below when going into production
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  
    server.begin();
}




void startStationTask(){
    xTaskCreate( stationTask, "stationTask", 3000, (void*) NULL, 1, NULL);
}

void stationTask(void * parameters){
  startStation();
  initializeStationReconnect();
  vTaskDelete(NULL);
}


void startStation(){
  WiFi.disconnect(true);
  WiFi.begin(ST_SSID, ST_PASSWORD);
  int reconnectionsLeft = RECONNECTION_MAX_COUNT;
  while(reconnectionsLeft > 0 && (WiFi.status() != WL_CONNECTED)){
    Serial.println("Connecting to <" + String(ST_SSID) + "> Status: " + String(WiFi.status()));
    if(WiFi.status() == WL_DISCONNECTED){
      WiFi.reconnect();
      vTaskDelay(RECONNECTION_TIMEOUT);
    }
    vTaskDelay(RECONNECTION_TIMEOUT);
    reconnectionsLeft--;
  }
  StationIPAddress = WiFi.localIP();
  if(WiFi.status() == WL_CONNECTED) Serial.println("Successfully connected to <" + String(ST_SSID) + ">. Local IP address: " + stringIPAddress(StationIPAddress));
  else Serial.println("Couldn't connect to <" + String(ST_SSID) + ">");
}


void initializeStationReconnect(){
  WiFi.onEvent(stationReconnectEvent, SYSTEM_EVENT_STA_DISCONNECTED); 
}


void stationReconnectEvent(WiFiEvent_t event, WiFiEventInfo_t info){
  startStation();
}
