void sdLog(String logString){
  File dataFile = SD.open("data.csv", FILE_WRITE);
  dataFile.println(logString);
  dataFile.flush();
  Serial.println("Printed to SD");
}

