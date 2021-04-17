void sendErrorToSerial(String message){
  for(;;){
    Serial.println("Error: " + message);
    delay(100); 
  }
}


void initializeRandomSeed(){
  randomSeed(analogRead(DISCONNECTED_PIN));
}


SemaphoreHandle_t initializeSemaphore(){
  SemaphoreHandle_t tempSemaphore = xSemaphoreCreateBinary();
  if( tempSemaphore != NULL) xSemaphoreGive(tempSemaphore);
  else sendErrorToSerial("Failed initializing semaphore!");
  return tempSemaphore;
}
