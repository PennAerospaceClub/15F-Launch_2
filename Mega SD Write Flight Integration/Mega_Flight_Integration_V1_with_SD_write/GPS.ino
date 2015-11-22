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

    String GPSdataString = String(lat) + ", " +  String(longit) + ", " +  String(currAlt);
  }
}

//4.2.2 GPS Boundary Box
//CHECK IF THE BALLOON IS IN THE BOUNDARY BOX
boolean inBdryBox() {
  if ((lat < maxLat) && (lat > minLat) && (longit < maxLong) && (longit > minLong) && (currAlt < 29000)) //HERE <-- allow us to adjust maximum wanted altitude
  {
    return true;
  }
  else
  {
    return false;
  }
}

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

String GPStoString(){
  String GPSdataString = String(lat) + "," + String(longit) + "," + String(currAlt);
}


