//4.4 Temperature Sensors: Current 
/*void readTempVoltage()
{
  float tempVoltage1 = analogRead(TEMP1_PIN);
  float tempVoltage2 = analogRead(TEMP2_PIN);

  File dataFile = SD.open("temperature.txt", FILE_WRITE);
  SD.open("temperature.txt", FILE_WRITE);
  if (dataFile) {
     dataFile.print(millis());
     dataFile.print(",");
     dataFile.print((String)tempVoltage1);
     dataFile.print(",");
     dataFile.print((String)tempVoltage2);
      
     dataFile.close();   
 }  
    // if the file isn't open, pop up an error:
   else {
     //Serial.print("temp text fail");
     digitalWrite(LED_YELLOW, HIGH);
     delay(100);
     digitalWrite(LED_YELLOW, LOW);
 }       
}
*/
