 // TODO Read last program from flash memory

void initializeProgramModes(){
  programModeSemaphore = initializeSemaphore();
  currentProgramMode = modeOFF;

}
