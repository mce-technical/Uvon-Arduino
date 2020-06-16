///efeefeoejofjfeofjeofjeofeofjej


#include <C:\Users\lenovo\Documents\Uvon-Arduino-STM32F103RTC6\firmware_V1.0\Ezo_uart.h>
#include <stdio.h>
#include <stdint.h>


#define power 12
#define LEDS 8
#define SSR1 7
#define SSR2 6
#define SSR3 5
#define SSR4 4


#define line_tresh 290
int State = 0;
int counter = 0;
char inputString[100]; // input buffer for receiving commands
bool stringComplete = false; // flag for command complete. true if received false otherwise
bool stringComplete2 = false; // flag for command complete. true if received false otherwise


int a0,a1,a2,a3,a4;

int dirr = 0; // direction of right side wheels. if 1 CW else if 0 CCW
int dirl = 0; // direction of left side wheels. if 1 CW else if 0 CCW

int speedr = 0; // speed of right side wheels. Range [-255, 255]
int speedl = 0; // speed of left side wheels. Range [-255, 255]

int line = 0;


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


void send_to_rdriver(int id, int dir)
{
  char buf[10];
  int first_byte = 235 - dir;
  int check_sum = first_byte ^ speedr;
  sprintf(buf,"%c%c%c",first_byte ,speedr,check_sum);
  Serial2.print(buf);
  
  delay(10);

  first_byte = 232 + dir;
  check_sum = first_byte ^ speedr;
  sprintf(buf,"%c%c%c",first_byte ,speedr,check_sum);
  Serial2.print(buf);
  delay(10);
}
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
{
  if(stringComplete)
  {
    if(inputString[0] != 'S')
      Serial.print(inputString);
    if(inputString[0] == 'M' && line == 0)
    {
      sscanf(inputString,"M%d,%d,%d,%d",&dirr,&speedr,&dirl,&speedl);
      char buf[10];

      //Right Side First Motor
      send_to_rdriver(235,dirr);
      //Right Side Second Motor
      send_to_ldriver(232,dirl);
      
    }
    else if(inputString[0] == '1')
    {
      digitalWrite(power, HIGH);
      delay(50);
      digitalWrite(power, LOW);
      State = 1;
      //Serial.println(inputString);
    }
    else if(inputString[0] == '2')
    {
      digitalWrite(power, HIGH);
      delay(50);
      digitalWrite(power, LOW);
      State = 0;
      //Serial.println(inputString);
    }
    else if(inputString[0] == '3')
    {
      digitalWrite(LEDS, HIGH);
      //delay(10);
      //Serial.println(inputString);
    }
    else if(inputString[0] == '4')
    {
      digitalWrite(LEDS, LOW);
      //delay(10);
      //Serial.println(inputString);
    }
    else if(inputString[0] == 'I')
    {
      if(inputString[1] == '1')
      {
        digitalWrite(SSR1, HIGH);
      }
      else if(inputString[1] == '0')
      {
        digitalWrite(SSR1, LOW);
      }
    }
    else if(inputString[0] == 'U')
    {
      if(inputString[1] == '1')
      {
        digitalWrite(SSR2, HIGH);
      }
      else if(inputString[1] == '0')
      {
        digitalWrite(SSR2, LOW);
      }
    }
    else if(inputString[0] == 'L')
    {
      line = inputString[1] - '0';
      if(line == 0)
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
    else if(inputString[0] == 'S')
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
