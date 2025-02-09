#include <Ps3Controller.h>
#include<Wire.h>
#define slaveAddress 0x08
// Variables for joystick values
byte Xi , Yi , CMD  = 0;
byte dataArray[3];

/* Data format :

    byte 1 : x coordinate (-127 to 127)
    byte 2 : y coordinate (-127 to 127)
    byte 3 :  0b00000000 : None
              0b10000000 : Lift up
              0b01000000 : Lift down
              0b00100000 : Pick up
              0b00010000 : Drop down
              0b00001000 : Rotate Left
              0b00000100 : Rotate Right
              0b00000010 : Spin suck
              0b00000001 : Stop suck
              0b00000011 : Spin flywheels
              0b00000101 : Stop flywheels
              0b00000111 : Start BLDC
              0b00001001 : Speed BLDC
              0b00001011 : Stop BLDC

*/


void setup()
{
  // Setup Serial Monitor for testing
  Serial.begin(115200);
  Wire.begin();
  // Define Callback Function
  Ps3.attach(notify);
  // Define On Connection Function
  Ps3.attachOnConnect(onConnect);
  // Emulate console as specific MAC address (change as required)
  Ps3.begin("52:1e:c8:7a:9f:03"); 
  // Print to Serial Monitor
  Serial.println("Ready.");
}

void loop()
{

  if (!Ps3.isConnected()) 
  {
    Xi = 0;
    Yi = 0;
    CMD = 0b00000000;
  }
  return;
}

// On Connection function
void onConnect() {
  // Print to Serial Monitor
  Serial.println("Connected.");
}

// Callback Function
void notify()
{
  // Get Joystick value
  Xi = (Ps3.data.analog.stick.rx);
  Yi = (Ps3.data.analog.stick.ry);

  //Get button value
  if (Ps3.data.button.up)   //Lift up
  {
    // Serial.print("Lift up");
    CMD = 0b10000000;
  }
  if (Ps3.data.button.down)   //Lift down
  {
    // Serial.print("Lift down");
    CMD = 0b01000000;
  }
  if (Ps3.data.button.triangle) //Pick up
  {
    // Serial.print("Pick up");
    CMD = 0b00100000;
  }
  if (Ps3.data.button.cross) //Drop down
  {
    // Serial.print("Drop down");
    CMD = 0b00010000;
  }
  if (Ps3.data.button.l1)    //Rotate left
  {
    // Serial.print("Rotate left");
    CMD = 0b00001000;
  }
  if (Ps3.data.button.r1)   //Rotate right
  {
    // Serial.print("Rotate right");
    CMD = 0b00000100;
  }
  if (Ps3.data.button.square)   //Spin suck
  {
    // Serial.print("Rotate right");
    CMD = 0b00000010;
  }
  if (Ps3.data.button.circle)   //Stop suck
  {
    // Serial.print("Rotate right");
    CMD = 0b00000001;
  }
  if (Ps3.data.button.left)   //Spin flywheels
  {
    // Serial.print("Spin flywheels");
    CMD = 0b00000011;
  }
  if (Ps3.data.button.right)   //Stop flywheels
  {
    // Serial.print("Stop flywheels");
    CMD = 0b00000101;
  }
  if (Ps3.data.button.l2)   //Start BLDC
  {
    // Serial.print("Start BLDC");
    CMD = 0b00000111;
  }
  if (Ps3.data.button.r2)   //Speed BLDC
  {
    // Serial.print("Speed BLDC");
    CMD = 0b00001001;
  }
  if (Ps3.data.button.start)   //Stop BLDC
  {
    // Serial.print("Stop BLDC");
    CMD = 0b00001011;
  }

  // Print to Serial Monitor
  dataArray[0] = Xi ;
  dataArray[1] = Yi ;
  dataArray[2] = CMD ;
    Serial.print("to Slave");
    Serial.print(" Xi : ");
    Serial.print(  (int8_t)Xi);
    Serial.print(" ,Yi : ");
    Serial.print((int8_t) Yi);
    Serial.print(" ,CMD : ");
    Serial.println(CMD ,BIN);
  Wire.beginTransmission(slaveAddress); //address is queued for checking if the slave is present
  Wire.write(dataArray, 3); //data bytes are queued in local buffer
  Wire.endTransmission(); //all the above queued bytes are sent to slave on ACK handshaking
  CMD = 0b00000000;

}
