//4.2 GPS
//4.2.1 Interfacing GPS
void initGPS() {
  GPSSerial.begin(9600);
}

String updateGPS()
{
  static int i = 0;

  while (GPSSerial.available())
  {
    char ch = GPSSerial.read();
    if (ch != '\n' && i < SENTENCE_SIZE)
    {
      sentence[i] = ch;
      i++;
    }
    else
    {
      sentence[i] = '\0';
      i = 0;
      return readGPS();
    }
  }  
  return "";
}

void updateMaxAlt()
{
  if (currAlt >= maxAlt){
    maxAlt = currAlt;
  }
  if(maxAlt > 100000){
    maxAlt = 0;
  }
}

//RETRIEVE & PARSE DATA FROM GPS
  String readGPS() {
  char field[20];
  getField(field, 0);
  //Serial.println(field); //Debug
  if (strcmp(field, "$GPGGA") == 0)
  {
    unsigned long latDEG = 0;
    unsigned long latMIN = 0;
    lat = 0;
    unsigned long longDEG = 0;
    unsigned long longMIN = 0;
    longit = 0;
    //Serial.println("---");

    getField(field, 2); // Latitude number in deg/min/sec
    for (int i = 0; i < 2; i++)
    {
      latDEG = 10 * latDEG + (field[i] - '0');
    }
    for (int i = 2; i < 9; i++)
    {
      if (field[i] != '.')
      {
        latMIN = 10 * latMIN + (field[i] - '0');
      }
    }
    lat = (latDEG * 100000 + latMIN / 6);

    getField(field, 3); // N

    getField(field, 4); // Longitude number in deg/min/sec
    for (int i = 0; i < 3; i++)
    {
      longDEG = 10 * longDEG + (field[i] - '0');
    }
    for (int i = 3; i < 10; i++)
    {
      if (field[i] != '.')
      {
        longMIN = 10 * longMIN + (field[i] - '0');
      }
    }
    longit = (longDEG * 100000 + longMIN / 6);

    getField(field, 5); // W

    for(int i=0;i<10;i++)
      field[i]=0;
    getField(field, 9); // Altitude number
    currAlt = 0;
    for (int i = 0; i < 8; i++)
    {
      if (field[i] <='9' && field[i]>='0')
      {
        currAlt = 10 * currAlt + (field[i] - '0');
      }
    }
    currAlt = currAlt / 10; //Marcos* -- Not sure if we should do this, loses the last decimal point, I know it's not important
    ////Serial.print(field); //182.2
    getField(field, 10); // Meters
    ////Serial.println(field); //m
    // Print lat, long, and alt in degree and decimal form
    //Serial.print("Lat: ");
    //Serial.print(lat);
    //Serial.print(" Long: ");
    //Serial.print(longit);
    //Serial.print(" Current Alt: ");
    //Serial.println(currAlt);

    //print data to SD
    //GPSSD();

    //Create a string with all of the GPS data

    String GPSdataString = String(lat) + ", " +  String(longit) + ", " +  String(currAlt);
  }
}
  
//PRINT GPS DATA TO SD
/*void GPSSD()
{
  if (GPSSerial.available())
  {
     String latstr = String(lat);
     String longitstr = String(longit);
     String currALTstr = String(currAlt);
     String GPSdataString = latstr + " " + longitstr + " " + currALTstr;
     
     File dataFile = SD.open("gps.txt", FILE_WRITE);
     delay(100);
     if (dataFile) {
      //  dataFile.println(dataString);

        delay(100); 
        dataFile.close();  
        delay(300); 
     }  
        // if the file isn't open, pop up an error:
     else {
        //Serial.print("gps text fail");
        digitalWrite(LED_YELLOW, HIGH);
        delay(100);
        digitalWrite(LED_YELLOW, LOW);
     }       
  }      
}
*/

//4.2.2 GPS Boundary Box
//CHECK IF THE BALLOON IS IN THE BOUNDARY BOX
boolean inBdryBox() {
  ////Serial.print("lat: "); //Serial.print(lat); //Serial.print(" max lat: "); //Serial.print(maxLat); //Serial.print(" min lat: "); //Serial.print(minLat); // Debug
  ////Serial.print("long: "); //Serial.print(longit); //Serial.print(" max long: "); //Serial.print(maxLong); //Serial.print(" min long: "); //Serial.print(minLong);
  ////Serial.print("currAlt: "); //Serial.print(currAlt);
  if ((lat < maxLat) && (lat > minLat) && (longit < maxLong) && (longit > minLong) && (currAlt < 29000)) 
  {
    ////Serial.println("I'm in bdry"); //Debug
    return true;
  }
  else
  {
    ////Serial.println("I'm not in bdry"); //Debug
    return false;
  }
}

//RETRIEVE ROW OF DATA FROM GPS MODULE
void getField(char* buffer, int index)
{
  int sentencePos = 0;
  int fieldPos = 0;
  int commaCount = 0;
  while (sentencePos < SENTENCE_SIZE)
  {
    if (sentence[sentencePos] == ',')
    {
      commaCount++;
      sentencePos++;
    }
    if (commaCount == index)
    {
      buffer[fieldPos] = sentence[sentencePos];
      fieldPos++;
    }
    sentencePos++;
  }
  buffer[fieldPos] = '\0';
}

//4.2.3 GPS Falling
//CHECK IF THE BALLOON HAS DESCENDED FROM ITS PEAK ALTITUDE
boolean isFalling() {
  if (currAlt + 500 < maxAlt)
  {
    return true;
  }
  else
  {
    return false;
  }
}

