

void setup() {
  Serial.begin(115200);
  initializeRandomSeed();
  
  initializeScreen();
  initializeDisplay();

  initializeRotaryEncoder();
  initializeRotaryEncoderInterrupt();
  startRotaryEncoderButtonTask();
  
  startScreenManagerTask();

  initializeServer();




  server.on("/ss", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(request->url() + " -- " + request->host());
    request->send(200, "text/html", "<!DOCTYPE HTML><body>hejj</body>");
  });



  startServer();



  // TODO This part doesn't work yet, but that may be because I'm connected to the uni network. TODO : Check later on proper router.
  /*
  WiFi.begin(ST_SSID, ST_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(WiFi.status());
    Serial.print("Connecting to <");
    Serial.print(ST_SSID);
    Serial.println(">");
    delay(500);
  }*/

  delay(1000);
  
  Serial.print("Esp32 soft ip: ");
  Serial.println(WiFi.softAPIP());

  //Serial.print("Esp32 local ip: ");
  //Serial.println(WiFi.localIP());


  
}



// dim(true/false) - przyciemnienie lekkie ekranu (spoko)
// display.setTextSize() - defaultowa czcionka ma rozmiar 5x8, czyli rozmiar 4 jest maximum (na wysokość)
// display.setTextWrap(w) - zmienia szerokosc "otoczki" dookola literek


void loop() {
}





//      https://github.com/adafruit/Adafruit_BusIO
//      https://github.com/adafruit/Adafruit-GFX-Library
//      https://github.com/adafruit/Adafruit_SSD1306
//      https://github.com/me-no-dev/ESPAsyncWebServer#why-should-you-care
//      https://github.com/me-no-dev/AsyncTCP
