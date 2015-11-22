void SDLog(String toWrite){
SD.open("data.txt", FILE_WRITE);
File dataFile = SD.open("imu.txt", FILE_WRITE);
dataFile.println(toWrite);
dataFile.close();
}

