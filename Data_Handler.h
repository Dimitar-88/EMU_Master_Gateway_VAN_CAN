TSS46X_VAN* VANInterface;
// Can Protocol Set up.
#define CAN0_INT 8  // Set INT
MCP_CAN CAN0(7);    // Set CS
#define CAN1_INT 6  // Set INT
MCP_CAN CAN1(5);    // Set CS

// Can Message Handler
char msgString[128]="";   // DEBUGING
unsigned char rxBuf[8];
long unsigned int rxId;
unsigned char len = 0;

// Gatewahy Type
byte Can_Can = 0;
byte Can_Van = 1;

byte Testing = 0;
byte Test = 0;
byte Test2= 0;

//1: dashboards made before 2004
//2: dashboards made after  2004
uint8_t dashboard_version = 1;

// ECU DATA 
short RPM         = 8000;
short Speed       = 0;                    // Detect Time between messages to find the relative distance 
int Speed_Time    = 0;                    // 
int Distance      = 0;                    // Calculate total
int Distance_Now  = 0;                    // Calculate since last reed 
int Service_KM    = 10000;                // 0xFA // Range Calculation // Service A 5100 = FF // Service B 5100 = 01
byte Service_A    = Service_KM/5100;
byte Service_B    = (Service_KM%5100)/20;
byte Hand_Brake   = 0;                    // 0 and 1 to show it is oN
byte Hand_Brake_Stop = 0;                 // 0 and 1 to show handbrake and stop light

byte Check_Engine           = 0;
byte Check_Engien_Flashing  = 0;
byte Air_Bag_Main           = 0;
byte Air_Bag_Side           = 0;
byte Air_Bags               = 0;
byte Airbag_Off             = 0;
byte Low_Fuel               = 0;
byte ESP_OFF                = 0;
byte Check_ESP              = 0;
byte Cgarge                 = 0;


// Temperatures Data
byte IAT          = 0;  // air temp     
int Water_Temp    = 0;  // Water Temperature
byte Oil_high_temp    = 0;                // 0 and 1 Show if it is High or not
byte Water_high_temp  = 0;                // 0 and 1 Show if it is High or not 
byte Hand_Oil_Temp    = 2;                // 02 OK , 04 handbrake

// Oil Data
byte Oil_temp     = 0;  // hex - 40 = actual value
byte Oil_Level    = 0;  // 25 = ______ 39 = OO____ 53 = OOO___ 67 = OOOO__ 81 = OOOOO_ >82 = OOOOOO
float Oil_Pressure= 0;

// Fuel Data
byte Fuel_Level   = 80; // in %
byte Lambda       = 0;  // Lambda

// Signaling
byte Left_Blinker = 0;  // 4 (active)
byte Right_Blinker= 0;  // 8 (active)
byte Hazards      = 0;  // C (active)
byte Low_Beam     = 0;  // 8x 
byte High_Beam    = 0;  // 4x
byte Front_Fog    = 0;  // 2x
byte Rear_Fog     = 0;  // 1x
byte Lights_Command = 0;// Actual Value send to the Dash

// Gear | Gearbox
byte Gear      = 0;
byte DSG_Mode  = 0;

// Flags Details
byte OFlags1 = 0; // PO1,PO2,PO3,PO4,PO5,VPO01,VPO02,VPO03
byte OFlags3 = 0; // 6- LC Map , 7 - ALS Map
byte OFlags4 = 0; // 6 - Boost Map
byte ERRFLAG = 0; // Errors and Check Engine light
byte Flags1  = 0; // Gearcut , ALS , LC , Idle , Table Set , TC - Active

void Send4FC_Data(){
  Service_A = Service_KM/5100;
  Service_B = (Service_KM%5100)/20;
  // FOglights Base
  Lights_Command = 0;
  if(rear_Fog == 1 && front_Fog == 1 )        {Lights_Command = 48;}
    else if(rear_Fog == 1)                    {Lights_Command = 16;}
    else if(front_Fog == 1)                   {Lights_Command = 32;}
  
  // Low and High Beams
  if(Lights_Command == 0){
    if(High_Beam == 1 && Low_Beam == 1)       {Lights_Command += 160;}
      else if(Low_Beam == 1)                  {Lights_Command += 128;}
      else if(High_Beam == 1)                 {Lights_Command += 64;}
  }
  else if(Lights_Command != 0){
    if(High_Beam == 1 && Low_Beam == 1)   {Lights_Command += 160;}
    else if(Low_Beam == 1)                    {Lights_Command += 128;}
    else if(High_Beam == 1)                   {Lights_Command += 64;}
  }
  
  // Blinkers
  if(Left_Blinker == 1 && Right_Blinker == 1 ){Lights_Command += 12;}
  else if(Left_Blinker == 1)                  {Lights_Command += 4;}
  else if(Right_Blinker == 1)                 {Lights_Command += 8;}
  
  // Fuel Lefrt Calculation
  Fuel_Level = analogRead(fuel_Level);
}

// Has data for Oil temp and Level
  // 1 - Dash Enabled 0x80
  // 2 - Should be door Open
  // 3 - KM Left efore Service 05 -100, km, 0A -200, 10 -300 ,15 -400, 20 -600
  // 6 - Lights Combo , x4 - Left x8 - Right xF- Both , 1x - Rear Fog 2x - Front FOg , 3x - Fog F+R , 4x-Long B, 8x - Short, Ax - Long + Short
  // 7 - Oil Temp
  // 8 - Fuel Level
  // 9 - Oil Level at Ignition
  //                              1     2        3          4       5     6       7          8           9       10    11    12    13    14
uint8_t dashboard_packet[14] = { 0x80, 0x01, Service_A, Service_B, 0x00, 0xFF, Oil_temp, Fuel_Level, Oil_Level, 0x00, 0xFE, 0x01, 0x00, 0x00 };
/* For dashboards made before 2004 */
void Send4FC_V1(uint8_t channelId) {
  Send4FC_Data();
  VANInterface->set_channel_for_transmit_message(channelId, 0x4FC, dashboard_packet, 11, 1);
}


/* For dashboards made after 2004 */
void Send4FC_V2(uint8_t channelId) {
  VANInterface->set_channel_for_transmit_message(channelId, 0x4FC, dashboard_packet, 14, 1);
}

void Send824(uint8_t channelId)
// 1 - RPM 0-FA
// 3 + 4 - Speed
//
{  //                     1     2     3     4     5     6     7
  uint8_t packet[7] = { 0x9F, 0x00, 0x00, 0xff, 0x01, 0x86, 0x00 };
  VANInterface->set_channel_for_transmit_message(channelId, 0x824, packet, 7, 0);
}
/*
    The message id 0x8FC is asked by the BSI. It asks the instrument cluster for its mileage.
    This way if a dashboard with a higher mileage is installed the BSI can update its value with the one from the cluster.
*/
void QueryInstrumentClusterForMileage(uint8_t channelId) {
  VANInterface->set_channel_for_reply_request_message(channelId, 0x8FC, 7, 1);
}

void Ack664(uint8_t channelId) {
  VANInterface->set_channel_for_receive_message(channelId, 0x664, 13, 1);
}

void Send8A4(uint8_t channelId, int temperature) {  
  // 1 - x0-xF Brightness , EX Fx - Lightning OFF , 6X Lighting ON
  // 2 - X5 - Battery Symbol On ( ignition ) , 07 No Lights , 06 - Handbrake and Battery ,
  // 3 - Colant Temp
  // 4 + 5 + 6 Odometer
  //                      1     2     3     4     5     6       7
  uint8_t packet[7] = { 0x6F, 0x07, 0x9F, 0x6D, 0x1F, 0x03, temperature * 2 + 0xFF };
  VANInterface->set_channel_for_transmit_message(channelId, 0x8A4, packet, 7, 0);
}

void Send524_Data(){
  Check_ESP = 0;
  Check_Engine = 0;
  Hand_Oil_Temp = 0;
  Air_Bags = 0;

  if(Oil_high_temp)               { Hand_Oil_Temp = 80; }
  else if(Water_high_temp)        { Hand_Oil_Temp = 40; }
  if(Hand_Brake = 1 && Speed != 0){ Hand_Oil_Temp +=4;  }
  if(ERRFLAG !=0)                 { Check_ESP = 1;      }
  if(ESP_OFF == 1)                { Check_ESP += 16;    }
  if(Air_Bag_Side == 1)           { Air_Bags = 1        }
  if(Air_Bag_Main == 1)           { Air_Bags = 2        }
  if(Fuel_Level <= 10)            { Low_Fuel = 90       }
  )
}  

void Send524(uint8_t channelId)
// 1 - 02 - OK , 08 Handbrake Stop , 80 - Water Temp High , 40 - Engine Oil Temp High
// 2 - Gti Not Relevant
// 3 - 01 - Check Engine , 10 - ESP ON , 
// 4 - 01 - Side Airbab , 02 - General Airgab Faultm 
// 5 - 20 - Charge Light on Dash , 80 - Check Engine Flashing
// 6 - 01 - Handbrake ON , 02 - Seat belt not ON , 03 Both , 04 passanger airbag Off , 
// 7 - 80 - Low Fuel
// 8 - not really relevant
// 9 - 02 ESP Inactive 
{  //                      1     2     3     4     5     6     7     8     9     10    11    12    13    14
  uint8_t packet[16] = { 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00 };

  uint8_t messageLength = 16;  //V2 dashboard has a message length of 16
  if (dashboard_version == 1) {
    messageLength = 14;
  }
  VANInterface->set_channel_for_transmit_message(channelId, 0x524, packet, messageLength, 0);
}