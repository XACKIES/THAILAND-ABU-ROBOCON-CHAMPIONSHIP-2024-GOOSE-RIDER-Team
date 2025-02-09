#include<Wire.h>
#include <Servo.h>

#define slaveAddress 0x08  //you have to assign an 8-bit address to Slave
byte dataArray[3];

Servo leftLiftServo, rightLiftServo, leftGripServo, rightGripServo, ESCLeft, ESCRight;

/* Data format :

    byte 1 : x coordinate (-127 to 127)
    byte 2 : y coordinate (-127 to 127)
    byte 3 :  0b00000000 : None
              0b10000000 : Lift up
              0b01000000 : Lift down
              0b00100000 : Grip
              0b00010000 : Ungrip
              0b00001000 : Rotate Left
              0b00000100 : Rotate Right
              0b00000010 : Spin suck
              0b00000001 : Stop suck
              0b00000111 : Start BLDC
              0b00001001 : Speed BLDC
              0b00001011 : Stop BLDC

*/
//-----------------------------------------------------------------------------------------

//Servo variables
int gripPos = 105;
int ungripPos = 70;
int liftPos = 30;
int unliftPos = 90;
int currentGripPos;
int currentLiftPos;
int startPos = 120;
int workPos = 150;

//BLDC variables
int staySpeed = 55;
int startSpeed = 60;
int minSpeed = 0;
int maxSpeed = 70;    //MAX = 180
int startState = 0;
int stayState = 0;
int speedState = 0;

int A, B, C, D ;
// Define motor driver pins
#define PWMA_PIN 2   //Front
#define AIN1_PIN 46
#define AIN2_PIN 47
#define PWMB_PIN 3
#define BIN1_PIN 48
#define BIN2_PIN 49
#define PWMC_PIN 4   //Rear
#define CIN1_PIN 50
#define CIN2_PIN 51
#define PWMD_PIN 5
#define DIN1_PIN 52
#define DIN2_PIN 53
#define PWME_PIN 6   //Suck motors
#define EIN1_PIN 22
#define EIN2_PIN 23

#define leftLiftServoPin 7    //Lift servos
#define rightLiftServoPin 8
#define leftGripServoPin 9    //Grip servos
#define rightGripServoPin 10

#define ESCLeftPin 11         //BLDC motors
#define ESCRightPin 12
#define PWMF_PIN 13           //Flywheel motors
#define FIN1_PIN 24
#define FIN2_PIN 25


float pi = 3.14159265358979;

// Variables for Motor PWM values
//int motorAPWM = 0;
//int motorBPWM = 0;
//int motorCPWM = 0;
//int motorDPWM = 0;
//int motorIdle = 45;

byte Xi , Yi , CMD ;
//int8_t rightX, rightY, leftX;

//for safety
int safetyfactor = 3;

//-----------------------------------------------------------------------------------------
// Motor movement function

///////////////////////////////////////////////////////////////// x in ( -255 to 255 ) ////////////////////////////////////////

void Wheel_A(int x , char R = 1 )
{
  x = x * R ;
  if (x > 0)
  {
    digitalWrite(AIN1_PIN, HIGH);
    digitalWrite(AIN2_PIN, LOW);
    analogWrite(PWMA_PIN , abs(x));
  }
  else if (x < 0)
  {
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, HIGH);
    analogWrite(PWMA_PIN , abs(x));
  }
  else
  {
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
    analogWrite(PWMA_PIN , 0);
  }
}

void Wheel_B(int x , char R = 1)
{
  x = x * R ;
  if (x > 0)
  {
    digitalWrite(BIN1_PIN, HIGH);
    digitalWrite(BIN2_PIN, LOW);
    analogWrite(PWMB_PIN , abs(x));
  }
  else if (x < 0)
  {
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, HIGH);
    analogWrite(PWMB_PIN , abs(x));
  }
  else
  {
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
    analogWrite(PWMB_PIN , 0);
  }
}

void Wheel_C(int x , char R = 1)
{
  x = x * R ;
  if (x > 0)
  {
    digitalWrite(CIN1_PIN, HIGH);
    digitalWrite(CIN2_PIN, LOW);
    analogWrite(PWMC_PIN , abs(x));
  }
  else if (x < 0)
  {
    digitalWrite(CIN1_PIN, LOW);
    digitalWrite(CIN2_PIN, HIGH);
    analogWrite(PWMC_PIN , abs(x));
  }
  else
  {
    digitalWrite(CIN1_PIN, LOW);
    digitalWrite(CIN2_PIN, LOW);
    analogWrite(PWMC_PIN , 0);
  }
}

void Wheel_D(int x , char R = 1)
{
  x = x * R ;
  if (x > 0)
  {
    digitalWrite(DIN1_PIN, HIGH);
    digitalWrite(DIN2_PIN, LOW);
    analogWrite(PWMD_PIN , abs(x));
  }
  else if (x < 0)
  {
    digitalWrite(DIN1_PIN, LOW);
    digitalWrite(DIN2_PIN, HIGH);
    analogWrite(PWMD_PIN , abs(x));
  }
  else
  {
    digitalWrite(DIN1_PIN, LOW);
    digitalWrite(DIN2_PIN, LOW);
    analogWrite(PWMD_PIN , 0);
  }
}



void Movement(int8_t x , int8_t y , byte w = 0)
{
  byte  wi = 25;
  if (w == 0b00001000)
  {
    Wheel_A(-wi);
    Wheel_B(wi);
    Wheel_C(-wi);
    Wheel_D(wi);
  }
  else if (w == 0b00000100)
  {
    Wheel_A(wi);
    Wheel_B(-wi);
    Wheel_C(wi);
    Wheel_D(-wi);
  }

  else
  {
    A = (x * cos(45 * pi / 180))  + (y * sin(45 * pi / 180)) ;
    B = (x * cos(135 * pi / 180)) + (y * sin(135 * pi / 180));
    C = (x * cos(135 * pi / 180)) + (y * sin(135 * pi / 180));
    D = (x * cos(45 * pi / 180))  + (y * sin(45 * pi / 180)) ;
    Serial.print("Motion Data | ");
    Serial.print(" A : ");
    Serial.print(A / safetyfactor);
    Serial.print(" ,B : ");
    Serial.print(B / safetyfactor);
    Serial.print(" ,C : ");
    Serial.print(C / safetyfactor);
    Serial.print(" ,D : ");
    Serial.println(D / safetyfactor);
    Wheel_A(A / safetyfactor);
    Wheel_B(B / safetyfactor);
    Wheel_C(C / safetyfactor);
    Wheel_D(D / safetyfactor);
  }
}

void RUN_ESC(int u)
{
  int angle = map(u, 0, 100, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
  ESCLeft.write(angle);
  ESCRight.write(angle);
}

void option(byte x)
{
  if (x == 0b10000000)
  {
    // Lift up
    for (currentLiftPos = unliftPos; currentLiftPos >= liftPos; currentLiftPos--)
    {
      leftLiftServo.write(currentLiftPos);
      rightLiftServo.write(180-currentLiftPos);
      delay(10);
    }
    Serial.println("Lift up");
  }

  else if (x == 0b01000000)
  {
    // Lift down
    for (currentLiftPos = liftPos; currentLiftPos <= unliftPos; currentLiftPos++)
    {
      leftLiftServo.write(currentLiftPos);
      rightLiftServo.write(currentLiftPos);
      delay(15);
    }
    Serial.println("Lift down");
  }

  else if (x == 0b00100000)
  {
    //Grip
    currentGripPos = gripPos;
    leftGripServo.write(currentGripPos);
    rightGripServo.write(currentGripPos);
    Serial.println("Gripping");
    delay(50);
  }
  else if (x == 0b00010000)
  {
    //Ungrip
    currentGripPos = ungripPos;
    leftGripServo.write(currentGripPos);
    rightGripServo.write(currentGripPos);
    Serial.println("UnGripping");
    delay(50);
  }
  else if (x == 0b00000010)
  {
    //Spin suck motors
    Serial.println("Spinning suck motors");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(EIN1_PIN, HIGH);
    digitalWrite(EIN2_PIN, LOW);
    analogWrite(PWME_PIN, 255);
    Serial.println("Start Suck motors");
    delay(50);
  }
  else if (x == 0b00000001)
  {
    //Stop suck motors
    Serial.println("Stop suck motors");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(EIN1_PIN, LOW);
    digitalWrite(EIN2_PIN, LOW);
    analogWrite(PWME_PIN, 0);
    Serial.println("Stop Suck motors");
    delay(50);
  }
  else if (x == 0b00000011)
  {
    //Spin flywheels
    Serial.println("Spin flywheels");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(FIN1_PIN, HIGH);
    digitalWrite(FIN2_PIN, LOW);
    analogWrite(PWMF_PIN, 255);
//    ESCLeft.write(90);  //vary from 0-180
//    ESCRight.write(90);
    delay(50);
  }
  else if (x == 0b00000101)
  {
    //Stop flywheels
    Serial.println("Stop flywheels");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(FIN1_PIN, LOW);
    digitalWrite(FIN2_PIN, LOW);
    analogWrite(PWMF_PIN, 0);
//    ESCLeft.write(0);  //vary from 0-180
//    ESCRight.write(0);
    delay(50);
  }
  else if (x == 0b00000111)
  {
    //Start BLDC
    //Start Phase
    if (startState == 0)
    {
      //Serial.println("Start state == 0");

      for (int i = 0; i <= startSpeed; i += 5)
      {
        RUN_ESC(i);
        //Serial.println(i);
        delay(1000);
      }
      startState = 1;
    }
    //Stay Phase
    else
    {
      if (startState == 1)
      {
        if (stayState == 0)
        {
          for (int i = startSpeed; i >= staySpeed; i -= 5)
          {
            RUN_ESC(i);
            //Serial.println(i);
            delay(100);
          }
          stayState = 1;
        }

        else if (stayState == 1)
        {
          //Serial.println("Stay state");
          RUN_ESC(staySpeed);
        }
      }
    }
  }
  else if (x == 0b00001001)
  {
    //Speed BLDC
    if (speedState == 0)
    {
      for (int i = staySpeed; i <= maxSpeed; i += 5)
      {
        RUN_ESC(i);
        //Serial.println(i);
        delay(100);
      }
      speedState = 1 ;
    }
    else if (speedState == 1)
    {
      RUN_ESC(maxSpeed);
    }
  }
  else if (x == 0b00001011)
  {
    //Stop BLDC
    startState = 0;
    stayState = 0;
    speedState = 0;
    RUN_ESC(0);
  }
  else
  {
    //    digitalWrite(LED_BUILTIN, LOW);
    //    digitalWrite(EIN1_PIN, LOW);
    //    digitalWrite(EIN2_PIN, LOW);
    //    analogWrite(PWME_PIN, 0);
    //    myservo.write(currenPos);
    //    delay(50);
  }
}


/////////////////////////////////////////////////////////////
void setup()
{
  Wire.begin(slaveAddress);
  Serial.begin(115200);
  Wire.onReceive(receiveEvent);//you need to declre it in setup() to receive data from Master
  // Setup servos
  leftLiftServo.attach(leftLiftServoPin);
  rightLiftServo.attach(rightLiftServoPin);
  leftGripServo.attach(leftGripServoPin);
  rightGripServo.attach(rightGripServoPin);

  // Attach the ESC on pins

  ESCLeft.attach(ESCLeftPin); // (pin, min pulse width, max pulse width in microseconds)
  ESCRight.attach(ESCRightPin); // (pin, min pulse width, max pulse width in microseconds)

  // Set motor controller pins as outputs
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);
  pinMode(PWMC_PIN, OUTPUT);
  pinMode(PWMD_PIN, OUTPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(CIN1_PIN, OUTPUT);
  pinMode(CIN2_PIN, OUTPUT);
  pinMode(DIN1_PIN, OUTPUT);
  pinMode(DIN2_PIN, OUTPUT);
  pinMode(PWMD_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Ready.");
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);
  digitalWrite(BIN1_PIN, LOW);
  digitalWrite(BIN2_PIN, LOW);
  digitalWrite(CIN1_PIN, LOW);
  digitalWrite(CIN2_PIN, LOW);
  digitalWrite(DIN1_PIN, LOW);
  digitalWrite(DIN2_PIN, LOW);
}

/////////////////////////////////////////////////////////////

void loop()
{
  Xi = dataArray[0];
  Yi = dataArray[1];
  CMD = dataArray[2];

  Serial.print("From Master");
  Serial.print(" Xi : ");
  Serial.print(int8_t(Xi));
  Serial.print(" ,Yi : ");
  Serial.print(int8_t(~Yi));
  Serial.print(" ,CMD : ");
  Serial.println(CMD , BIN);

  Movement((Xi), (~Yi), CMD);
  option(CMD);

  //  Serial.print(" - Motor A = ");
  //  Serial.print(motorAPWM);
  //  Serial.print(" - Motor B = ");
  //  Serial.print(motorBPWM);
  //  Serial.print(" - Motor C = ");
  //  Serial.print(motorCPWM);
  //  Serial.print(" - Motor D = ");
  //  Serial.print(motorDPWM);

}



//-----------------------------------------------------------------------------------------


void receiveEvent(int howmany) //howmany = Wire.write()executed by Master
{
  //int i =0;
  while (1 < Wire.available())
  {
    for (int i = 0; i < howmany; i++)
    {
      dataArray[i] = Wire.read();
    }
    // dataArray[i] = Wire.read();
    //  i++;
  }
}

/////////////////////////////////////////////////////////////
