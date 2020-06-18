
///efeefeoejofjfeofjeofjeofeofjej

// EZO IR Color sensor library.
#include <C:\Users\lenovo\Documents\Uvon-Arduino-STM32F103RTC6\firmware_V1.0\Ezo_uart.h> 
//Standard library.
#include <stdio.h>
#include <stdint.h>

// Physical GPIO ports numbers.
#define power 12
#define LEDS 8
#define SSR1 7  
#define SSR2 6
#define SSR3 5
#define SSR4 4


#define line_tresh 290
int State = 0;
int counter = 0;
char inputString[100]; // input buffer for receiving commands from Jetson nano
bool stringComplete = false; // flag for command complete. true if received false otherwise
bool stringComplete2 = false; // flag for command complete. true if received false otherwise

bool Status_SSR_1 = false;//This varible check if the SSR turned on/off to blink LED's. 
bool Status_SSR_2 = false;//This varible check if the SSR turned on/off to blink LED's. 
bool couter_for_LED = false;
int timer_for_LED_on_off =0;

int a0,a1,a2,a3,a4;

int dirr = 0; // direction of right side wheels. if 1 CW else if 0 CCW
int dirl = 0; // direction of left side wheels. if 1 CW else if 0 CCW

int speedr = 0; // speed of right side wheels. Range [-255, 255]
int speedl = 0; // speed of left side wheels. Range [-255, 255]

int line = 0; // Line tracking status variable. If line is = to 1- in line tracking mode,if 0 - line tracking mode is off.


char answer[20];
int b = 0;

char sens_data[100];
Ezo_uart rgb(Serial1);


void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200); 
  Serial3.begin(115200);
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(power, OUTPUT);        // set power pin to output
  digitalWrite(power, LOW);      // Initial state LOW
  pinMode(LEDS, OUTPUT);         // set LEDS pin to output
  digitalWrite(LEDS, LOW);       // Initial state LOW
  pinMode(SSR1, OUTPUT);         // set LEDS pin to output
  digitalWrite(SSR1, LOW);       // Initial state LOW
  pinMode(SSR2, OUTPUT);         // set LEDS pin to output
  digitalWrite(SSR2, LOW);       // Initial state LOW

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  rgb.init_module(0,0);
  
  // reserve 200 bytes for the inputString:
  //inputString.reserve(200);
 // memset(inputString,0,200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stringComplete2 == true)
  {
    Serial.println(answer);
    //Serial.println("OK");
    stringComplete2 = false;
  }
  /*
  if(rgb.data_available())
  {
    Serial.print(rgb.get_reading());
  }
  */
  rgb.print_reading();
  //Serial1.println("R\r");
  
  //Serial1.readBytes(sens_data,6);
  // Serial.print(sens_data);
  command();
  //Led_blink();
  if(line)
  {
    LineTracking();
  }
  a0 = analogRead(A0);
  a1 = analogRead(A1);
  a2 = analogRead(A2);
  a3 = analogRead(A3);
  a4 = analogRead(A4);  
#if 0
  Serial.print(a0);
  Serial.print(" ");
  Serial.print(a1);
  Serial.print(" ");
  Serial.print(a2);
  Serial.print(" ");
  Serial.print(a3);
  Serial.print(" ");
  Serial.print(a4);
  Serial.print('\n');
#endif
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  //Serial.println("yes");
  static int i = 0;
  if(stringComplete == false)
  {
    while (Serial.available())
    {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString[i] = inChar;
      i++;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n')
      {
        stringComplete = true;
        i = 0;
      }
    }
  }
}
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent2() {
  //Serial.println("yes");
  static int i = 0;
  if(stringComplete2 == false)
  {
    while (Serial2.available())
    {
      // get the new byte:
      char inChar = (char)Serial2.read();
      // add it to the inputString:
      answer[i] = inChar;
      i++;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n')
      {
        stringComplete2 = true;
        i = 0;
      }
    }
  }
}
//This function sends commands to MCB of rights pairs to drive wheels.
//Q: you have declare the integer "id" but did't use it in "send_to_rdriver()" function, why?
void send_to_rdriver(int id, int dir)
{
  char buf[10];//The array of 10 for storeing the data that need to be send to motor controlling boards. 
  int first_byte = 235 - dir;//what is first_byte ? it need to be "id-dir".
  //The ^ (bitwise XOR) in C or C++ takes two numbers as operands and does XOR on every bit of two numbers. The result of XOR is 1 if the two bits are different.
  //check_sum is a byte? that send to MCB to check if the received data is correct.
  int check_sum = first_byte ^ speedr;
  sprintf(buf,"%c%c%c",first_byte ,speedr,check_sum);//stores the data in buffer- array of 10.
  Serial2.print(buf);// Sends command via UART 2 port to MCB.
  delay(10);// Wait 10 ms.

  //Is this same for 2nd wheel ?
  first_byte = 232 + dir;
  check_sum = first_byte ^ speedr;
  sprintf(buf,"%c%c%c",first_byte ,speedr,check_sum);
  Serial2.print(buf);
  delay(10);
}
//This function sends commands to MCB of left pairs to drive wheels.
void send_to_ldriver(int id, int dir)
{
  char buf[10];
  int first_byte = 238 + dir;
  int check_sum = first_byte ^ speedl;
  sprintf(buf,"%c%c%c",first_byte ,speedl,check_sum);
  Serial3.print(buf);

  delay(10);

  first_byte = 237 - dir;
  check_sum = first_byte ^ speedl;
  sprintf(buf,"%c%c%c",first_byte ,speedl,check_sum);
  Serial3.print(buf);
  delay(10);
}
void LineTracking()
{
  #if 1
  dirr = 0;
  dirl = 0;
  if(a2 > line_tresh)
    {
      speedr = 30;
      speedl = 15;
      send_to_rdriver(235,dirr); 
      send_to_ldriver(232,dirl);
    }
    else if(a3 > line_tresh)
    {
      speedr = 15;
      speedl = 30;
      send_to_rdriver(235,dirr); 
      send_to_ldriver(232,dirl);
    }
    else
    {
      speedr = 30;
      speedl = 30;
      send_to_rdriver(235,dirr); 
      send_to_ldriver(232,dirl);
    }
  #endif
}
void command()
{ //stringComplete is ture if some data form serial port (Jetson nano). Look on "void serialEvent(){}" function. 
  if(stringComplete)
  {// inputString[] is an array where data is collected from serila port(Jetson nano).
   // Statement above means > If first byte is not equal to "S". "S" command means that we need to send back the line position information. Look the end of command() function.
    if(inputString[0] != 'S')
      Serial.print(inputString);// Sends back received command.
    if(inputString[0] == 'M' && line == 0)// if first byte is = to 'M' and line tracking is off.
    { // sscanf() function is subtract the data parts of inputString and stores them into  dirr , speedr .... variables
      // %d is type of the data that need to be subtracted form inputString. %d is signed decimal integer.
      // "dirr" -is a direction of right pairs of wheels.
      // "sppedr" -is a direction of left pairs of wheels.
      // "dirl" and "speedl" are the same for left pairs.
      sscanf(inputString,"M%d,%d,%d,%d",&dirr,&speedr,&dirl,&speedl);
      char buf[10]; // why have you declare the buffer for MCB send data here?

      //Right Side First Motor
      //send_to_rdriver(id_of_motor_controlling_board,direction) is function that sends data to motor controlling board that controls right pairs of wheels.
      send_to_rdriver(235,dirr);
      //The same as send_to_rdriver but for left pairs.
      //Right Side Second Motor
      send_to_ldriver(232,dirl);
      
    }// If first byte of command is "1" then turn on motors.
    else if(inputString[0] == '1')
    { 
      digitalWrite(power, HIGH);// "power" is 12 pin connected to relay that turns on/off MCB's.
      delay(50);//wait 15 ms.
      digitalWrite(power, LOW);
      State = 1;// Changes on/off state to on.
      //Serial.println(inputString);
      digitalWrite(LEDS, HIGH);//Turns on LED's.
    }// If first byte of command is "0" then turn off motors.
    else if(inputString[0] == '2')
    {
      digitalWrite(power, HIGH);
      delay(50);
      digitalWrite(power, LOW);
      State = 0;//Turns off LED's.
      digitalWrite(LEDS, LOW);
      //Serial.println(inputString);
    }// if the command starts with "I" it regards to 1st SSR (DC SSR).
    else if(inputString[0] == 'I')
    {
      if(inputString[1] == '1')//if 2nd byte is 1 turn on.
      {
        digitalWrite(SSR1, HIGH);
        Status_SSR_1 = true;
      }
      else if(inputString[1] == '0')//if 2nd byte is 0 turn off.
      {
        digitalWrite(SSR1, LOW);
        Status_SSR_1 = false;
      }
    }// if the command starts with "U" it regards to 2nd SSR (AC SSR).
    else if(inputString[0] == 'U')
    {
      if(inputString[1] == '1')//if 2nd byte is 1 turn on.
      {
        digitalWrite(SSR2, HIGH);
        Status_SSR_2 = true;
      }
      else if(inputString[1] == '0')//if 2nd byte is 0 turn off.
      {
        digitalWrite(SSR2, LOW);
        Status_SSR_2 = false;

      }
    }else if(inputString[0] == 'L')// If received 1st byte command is 'L'.
    {
      line = inputString[1] - '0';
      if(line == 0)// And Second byte is 0. Ture off line tracking.
      {
        speedr = 1;
        speedl = 1;
        dirr = 1;
        dirl = 1;
        //Right Side First Motor
        send_to_rdriver(235,dirr);
        //Right Side Second Motor
        send_to_ldriver(232,dirl);
      }
    }
    else if(inputString[0] == 'S')// If "S" is received, send on "line" status.
    {
      if(a0 > line_tresh && a1 > line_tresh && a2 < line_tresh && a3 < line_tresh && a4 < line_tresh)
        Serial.print("1\n");
      else 
        Serial.print("0\n");
    }

   // Serial.print("\n");
    memset(inputString,0,strlen(inputString)); // Clears the command buffer
    stringComplete = false;  // Clears Command received flag
  }
}
void Led_blink(){
   // When bouth SSR's are turned on start blinking LED's.
   if(Status_SSR_1 && Status_SSR_2){
     if(timer_for_LED_on_off >= 1000){
     if(couter_for_LED){
       digitalWrite(LEDS, HIGH);//Turn's on LED's.
       couter_for_LED == false;
       }else
       {
         digitalWrite(LEDS, LOW);//Turn's on LED's.
         couter_for_LED = true;
       }
       timer_for_LED_on_off=0;
     }
     timer_for_LED_on_off++;
   }

}
