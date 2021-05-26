void initializeRotaryEncoder() {
  pinMode(ROTARY_ENCODER_BUTTON_PIN,  INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_A,       INPUT_PULLDOWN);
  pinMode(ROTARY_ENCODER_PIN_B,       INPUT_PULLDOWN);
  rotaryEncoderInputQueue = initializeQueue(ROTARY_ENCODER_INPUT_QUEUE_LENGTH, sizeof( struct RotaryEncoderInputCommand ) );
}


void initializeRotaryEncoderInterrupt() {
  attachInterrupt(ROTARY_ENCODER_PIN_A, rotaryEncoderInterrupt, RISING);
}


void rotaryEncoderInterrupt() {
  int pinBState = digitalRead(ROTARY_ENCODER_PIN_B);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  BaseType_t xInterfaceToService = NULL;

  xTimerPendFunctionCallFromISR( rotaryEncoderInterruptInISR, (void*)pinBState, ( uint32_t) xInterfaceToService, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR();
}


void rotaryEncoderInterruptInISR(void *pvParameter1, uint32_t ulParameter2) {
  BaseType_t xInterfaceToService = (BaseType_t) ulParameter2;

  if (rotaryEncoderInterruptTaskHandle != NULL) {
    vTaskDelete(rotaryEncoderInterruptTaskHandle);
  }

  int pinBState = (int) pvParameter1;
  xTaskCreatePinnedToCore( rotaryEncoderDebouncedHandler, "debouncedTask", 1000, (void*) pinBState, 1, &rotaryEncoderInterruptTaskHandle, 0 );
}


void rotaryEncoderDebouncedHandler(void *pinBState) {
  vTaskDelay(rotaryEncoderDebounceTime);

  portMUX_TYPE mutexStop = portMUX_INITIALIZER_UNLOCKED;
  taskENTER_CRITICAL(&mutexStop); // It's here to stop this task from being deleted, while sending command to queue.

    if ((int)pinBState == LOW) {
      RotaryEncoderInputCommand command = {right};
      sendRotaryEncoderInputCommand(command);
    }
    else { 
      RotaryEncoderInputCommand command = {left};
      sendRotaryEncoderInputCommand(command);
    }

  taskEXIT_CRITICAL(&mutexStop);

  rotaryEncoderInterruptTaskHandle = NULL;
  vTaskDelete(NULL);
}


void startRotaryEncoderButtonTask() {
  xTaskCreate( rotaryEncoderButtonTask, "rotaryEncoderButtonTask", 3000, (void*) NULL, 1, NULL);
}


void rotaryEncoderButtonTask( void * parameters ) {

  portTickType pollingUpdateTick = xTaskGetTickCount();
  int frameTime = 1000 / ROTARY_ENCODER_BUTTON_POLLING_RATE;
  boolean oldState = false;
  boolean newState;

  while (true) {
    newState = (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == HIGH);
    if (newState == HIGH && oldState == false) {
      vTaskDelay(frameTime / 2);
      newState = (digitalRead(ROTARY_ENCODER_BUTTON_PIN) == HIGH);
    }
    if (newState != oldState) {
      if(!oldState){
        RotaryEncoderInputCommand command = {buttonPress};
        sendRotaryEncoderInputCommand(command);
      }
      oldState = newState;
    }
    vTaskDelayUntil( &pollingUpdateTick, frameTime);
  }
  vTaskDelete(NULL);
}


void sendRotaryEncoderInputCommand(RotaryEncoderInputCommand command) {
  xQueueSend( rotaryEncoderInputQueue, &command, ( TickType_t ) 0 );
}
