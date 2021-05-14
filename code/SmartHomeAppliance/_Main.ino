



void setup() {
  Serial.begin(115200);
  initializeRandomSeed();
  initializeProgramModes();

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

  //ledcSetup(0, 50000, 8);
  //ledcAttachPin(16, 0);
  //ledcWrite(0, 100);


  //stripRGB.setBrightness(16);

  /*
  for(int j=0; j<10000; j++){
    stripRGB.clear();
    for(int i=0; i<NEOPIXEL_PIXELS; i++){
      int hue = (65536 * ((i + j)%NEOPIXEL_PIXELS)) / NEOPIXEL_PIXELS;
      //double saturation = 0.5;
      //double value = 0.5;

      
      
      //int red = max(0.0, 255 * sin((TWO_PI * i / NEOPIXEL_PIXELS) + TWO_PI / 3));
      //int green = max(0.0, 255 * sin((TWO_PI * i / NEOPIXEL_PIXELS)));
      //int blue = max(0.0, 255 * sin((TWO_PI * i / NEOPIXEL_PIXELS) - TWO_PI / 3));

      
      //stripRGB.setPixelColor(i, red, green, blue);

      stripRGB.setPixelColor(i, stripRGB.gamma32(stripRGB.ColorHSV(hue)));
      //Serial.println("Pixel nr. " + String(i) + ": Color: (" + String(red) + ", " + String(green) + ", " + String(blue) + ")");
    }
    stripRGB.show();
    delay(50);
  }*/







  initializeLEDController();
  startLEDControllerTask();

  
}



// dim(true/false) - przyciemnienie lekkie ekranu (spoko)
// display.setTextSize() - defaultowa czcionka ma rozmiar 5x8, czyli rozmiar 4 jest maximum (na wysokość)
// display.setTextWrap(w) - zmienia szerokosc "otoczki" dookola literek


void loop() {
}
