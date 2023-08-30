// Ecumaster Stream Define
int Can_BaseID = 1792; // 700hex

// Internal Power for Save
#define Power Pin     11 // Pin that leaves 12 V for some time so taht the MCU can save data in EEPROM

// Engine Related
#define oil_Level     1  //
#define fuel_Level    2  //

// Lighting set  definition
#define left_Blinker  3  // 4 (active)
#define right_Blinker 4  // 8 (active)
#define hazards       5  // C (active)
#define low_Beam      6  // 8x 
#define high_Beam     7  // 4x
#define front_Fog     8  // 1x
#define rear_Fog      9  // 2x

// Interlan Controlls
#define Handbrake     10  // 08


// 1 - 02 - OK , 08 Handbrake Stop , 80 - Water Temp High , 40 - Engine Oil Temp High
// 2 - Gti Not Relevant
// 3 - 01 - Check Engine , 10 - ESP ON , 
// 4 - 01 - Side Airbab , 02 - General Airgab Faultm 
// 5 - 20 - Charge Light on Dash , 80 - Check Engine Flashing
// 6 - 01 - Handbrake ON , 02 - Seat belt not ON , 03 Both , 04 passanger airbag Off , 
// 7 - 80 - Low Fuel
// 8 - not really relevant
// 9 - 02 ESP Inactive 

// High Value indiaction
#define High_Water_Temp   130
#define High_Oil_Temp     130

