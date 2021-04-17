void setup() {
  Serial.begin(115200);
  initializeRandomSeed();
  
  initializeScreen();
  initializeDisplay();

  initializeRotaryEncoder();
  initializeRotaryEncoderInterrupt();
  startRotaryEncoderButtonTask();
  
  startScreenManagerTask();
}


// dim(true/false) - przyciemnienie lekkie ekranu (spoko)
// display.setTextSize() - defaultowa czcionka ma rozmiar 5x8, czyli rozmiar 4 jest maximum (na wysokość)
// display.setTextWrap(w) - zmienia szerokosc "otoczki" dookola literek


void loop() {
}
