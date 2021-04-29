

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


  server.on("/ss", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(request->url() + " -- " + request->host());
    request->send(200, "text/html", "<!DOCTYPE HTML><body>hejj</body>");
  });


  startServer();
  startStationTask();

  //ledcSetup(0, 50000, 8);
  //ledcAttachPin(16, 0);
  //ledcWrite(0, 100);
}



// dim(true/false) - przyciemnienie lekkie ekranu (spoko)
// display.setTextSize() - defaultowa czcionka ma rozmiar 5x8, czyli rozmiar 4 jest maximum (na wysokość)
// display.setTextWrap(w) - zmienia szerokosc "otoczki" dookola literek


void loop() {
}
