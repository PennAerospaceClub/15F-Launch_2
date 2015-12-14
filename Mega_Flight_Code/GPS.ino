void readGPS(){
  gooddata = false;
  while(!gooddata){
  while(Serial1.available() && gpsMaxSearchTime > millis()){
    if((char)Serial1.read() == '$'){
      int i = 0;
      while(i < 75){
        if(Serial1.available()){
          field += (char)Serial1.read();
          i++;
        }
      }
      //Serial.println(field);
      if(field.startsWith("GPGGA")){
        //Serial.println(field);
        gooddata = true;

        int index1 = field.indexOf(",");
        index1 = field.indexOf(",", index1 + 1);
        int index2 = field.indexOf(",", index1 + 1);
        
        slat = field.substring(index1 + 1, index2);
        slat.remove(4, 1);

        index1 = index2;
        index1 = field.indexOf(",", index1 + 1);
        index2 = field.indexOf(",", index1 + 1);

        slongit = field.substring(index1 + 1, index2);
        slongit.remove(0, 1); slongit.remove(4, 1);

        index1 = index2;
        index1 = field.indexOf(",", index1 + 1);
        index1 = field.indexOf(",", index1 + 1);
        index1 = field.indexOf(",", index1 + 1);
        index1 = field.indexOf(",", index1 + 1);
        index2 = field.indexOf(".", index1 + 1);
        
        salt = field.substring(index1 + 1, index2);

        lat = slat.toInt();
        longit = slongit.toInt();
        alt = salt.toInt();
        
        field = "";
      }
      else{
        field = "";
      }
    }
    else{
      field = "";
    }
  }
  gpsMaxSearchTime = millis() + 5000;
  }
}

void updateMaxAlt()
{
  if (alt >= maxAlt){
    maxAlt = alt;
  }
}
