void ReadData() {
      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
      if(Testing != 0){
        sprintf(msgString, "Standard ID: 0x%.3lX DLC:  %1d  Data:", rxId, len);
        Serial.print(msgString);
        Serial.print  (rxBuf[0],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[1],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[2],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[3],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[4],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[5],HEX);
        Serial.print  (" ");
        Serial.print  (rxBuf[6],HEX);
        Serial.print  (" ");
        Serial.println(rxBuf[7],HEX);
      }

  // UDS Frames Response
  if (rxId == Can_BaseID) {   // Base ID of Ecumaster 
    RPM = rxBuf[0] + rxBuf[1];// 0-16000
    IAT = rxBuf[3];           // -40->127
  }
  else if (rxId == Can_BaseID+1) {
    // no Data Needed for my case
  }
  else if (rxId == Can_BaseID+2) {
      Speed         = rxBuf[0];
      Distance_Now  = Speed*(millis()-Speed_Time);
      Distance      += Distance_Now;
      Speed_Time    = millis();
      Oil_temp      = rxBuf[3];
      Oil_Pressure  = rxBuf[4]*0.0625;     // in bar
      Water_Temp    = rxBuf[6] + rxBuf[7]; // - 40 -> 250
      if(Oil_temp>High_Oil_Temp) {
        Oil_high_temp = 1 ;
      }
      if(Water_Temp>High_Water_Temp) {
        Water_high_temp = 1 ;
      }

  }
  else if (rxId == Can_BaseID+3) {
      Lambda        = rxBuf[2]*0.0078125; 
  }
  else if (rxId == Can_BaseID+4) {
      Gear          = rxBuf[0];             // Gear Selected
      ERRFLAG       = rxBuf[4] + rxBuf[5];  // Errors and Check Engine light
      Flags1        = rxBuf[6];             // Gearcut , ALS , LC , Idle , Table Set , TC - Active
  }
  else if (rxId == Can_BaseID+5) {

  }
  else if (rxId == Can_BaseID+6) {
      // OUTFLAGS 1-4 position 4,5,6,7
      OFlags1 = rxBuf[4]; // PO1,PO2,PO3,PO4,PO5,VPO01,VPO02,VPO03
      OFlags3 = rxBuf[6]; // 6- LC Map , 7 - ALS Map
      OFlags4 = rxBuf[7]; // 6 - Boost Map
  }
  else if (rxId == Can_BaseID+7) {
      DSG_Mode      = rxBuf[3]; // 2 = P , 3 = R , 4 = N , 5 = D , 6 = S , 7 = M , 15 = Fault
  }
}