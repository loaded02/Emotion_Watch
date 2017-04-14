/*
 *  All Serial Handling Code, 
 *  It's Changeable with the 'serialVisual' variable
 *  Set it to 'true' or 'false' when it's declared at start of code.  
 */

void serialOutputData(){   // Decide How To Output Serial. 
 if (serialVisual == true){  
    arduinoSerialMonitorVisual();   // goes to function that makes Serial Monitor Visualizer
 } else{
    sendDataToSerial();
 }        
}


//  Decides How To OutPut BPM and IBI Data
void serialOutputWhenBeatHappens(){    
 if (serialVisual == true){            //  Code to Make the Serial Monitor Visualizer Work
    Serial.print("*** Heart-Beat Happened *** ");  //ASCII Art Madness
    Serial.print("BPM: ");
    Serial.println(BPM);
 }  
}

void serialOutputWhenMoving() {
  if (serialVisual == true){
    Serial.println("###### Now Moving ######");  //ASCII Art Madness
  }
}



//  Sends Data to Pulse Sensor Processing App, Native Mac App, or Third-party Serial Readers. 
void sendDataToSerial(){
  Serial.print(tCoh * 10);
  Serial.print(" ");
  Serial.print(AvGsrSignal);
  Serial.print(" ");
  Serial.println(AvInMotion * 100);               
}


//  Code to Make the Serial Monitor Visualizer Work
void arduinoSerialMonitorVisual(){    
  Serial.println("--------------------");
  Serial.print("HFR CohVal: ");
  Serial.println(tCoh);
  Serial.print("minGsrSignal: ");
  Serial.println(minGsrSignal);
  Serial.print("maxGsrSignal: ");
  Serial.println(maxGsrSignal);
  Serial.print("AvGsrSignal: ");
  Serial.println(AvGsrSignal);
  Serial.print("AvInMotion: ");
  Serial.println(AvInMotion);
}


