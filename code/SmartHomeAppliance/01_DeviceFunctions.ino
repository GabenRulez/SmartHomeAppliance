void sendErrorToSerial(String message) {
  for (;;) {
    Serial.println("Error: " + message);
    delay(100);
  }
}


void initializeRandomSeed() {
  randomSeed(analogRead(DISCONNECTED_PIN));
}


SemaphoreHandle_t initializeSemaphore() {
  SemaphoreHandle_t tempSemaphore = xSemaphoreCreateBinary();
  if ( tempSemaphore != NULL) xSemaphoreGive(tempSemaphore);
  else sendErrorToSerial("Failed initializing semaphore!");
  return tempSemaphore;
}


QueueHandle_t initializeQueue(int queueLength, int sizeOfCell) {
  QueueHandle_t queue = xQueueCreate(queueLength, sizeOfCell);
  if (queue == NULL) {
    sendErrorToSerial("Failed initializing queue!");
  }
  return queue;
}


String stringIPAddress(IPAddress address) {
  return String(address[0]) + "." + String(address[1]) + "." + String(address[2]) + "." + String(address[3]);
}


void taskSleep(int miliseconds){
  vTaskDelay( miliseconds / portTICK_PERIOD_MS);
}


int normalizeBetween(int value, int from, int to){
  if(to > from){
    if(value < from) value = from;
    else if(value > to) value = to;
  }
  return value;
}


int mapNumber(int value, int valueFrom, int valueTo, int resultFrom, int resultTo){
  return (value - valueFrom) * (resultTo - resultFrom) / (valueTo - valueFrom) + resultFrom;
}
