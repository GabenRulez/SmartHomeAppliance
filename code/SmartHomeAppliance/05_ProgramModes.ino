 // TODO Read last program from flash memory

void initializeProgramModes(){
  programConfigSemaphore = initializeSemaphore();
  programConfig.currentMode = modeOFF;

}
