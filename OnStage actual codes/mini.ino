/*
  Software serial multiple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define K 0x000000//BLACK
#define B 0x0048ff//BLUE
#define G 0x008000//GREEN
#define P 0x9d00d2//PURPLE
#define R         0xFF0000//RED
#define O         0xFFA500//ORANGE
#define S 0xFFC0CB//ROSE
#define Y 0xFf6400//YELLOW
#define W 0xFFFFFF//WHITE
#define rightPin1 13
#define rightPin2 12
#define leftPin1 7
#define leftPin2 8
#define PIN 9// Pin of controle
#define LED_COUNT 79// Number of leds
//Cria uma instância da Classe Adafruit NeoPixel chamado "leds"
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial XBee(3, 4); // RX DOUT, TX DIN
Servo le, ri;
void fowardsMini()
{
  PORTD = ~((~PORTD)|(1<<leftPin1));//the command of type a = a|(1<<i) turns on the i-th bit in a
  PORTB = ~((~PORTB)|(1<<(rightPin1 - 8)));
  PORTB = ((PORTB)|(1<<(leftPin2 - 8))|(1<<(rightPin2 - 8)));//the command of type a = ~((~a)|(1<<i)) turns off the i-th bit in a
}
void stopMini()
{
  PORTB = ~((~PORTB)|(1<<(leftPin2 - 8))|(1<<(rightPin2 - 8)));//the command of type a = ~((~a)|(1<<i)) turns off the i-th bit in a
}
void raiseFlag(){
  le.write(105);
  ri.write(85);
}
void setup() {
  // Open serial communications and wait for port to open:
  leds.begin(); // initiates the LED strip
  leds.show(); // updates  the state of the LEDS

  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  le.attach(10);
  ri.attach(11);
  le.write(5);
  ri.write(175);
  delay(1000);
  analogWrite(5, 237);
  analogWrite(6, 255); //minha direita
  // set the data rate for the SoftwareSerial port
  XBee.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  PORTD = ~((~PORTD)|(1<<leftPin1));//the command of type a = a|(1<<i) turns on the i-th bit in a
  PORTB = ~((~PORTB)|(1<<(rightPin1 - 8)));
  PORTB = ~((~PORTB)|(1<<(leftPin2 - 8))|(1<<(rightPin2 - 8)));//the command of type a = ~((~a)|(1<<i)) turns off the i-th bit in a

}
void loop() { // run over and over
  if (XBee.available()) //he tests if it received the character "H"
  { 
    //digitalWrite(LED, HIGH);
      //delay(500);
    char c = XBee.read();
    //Serial.println(c);
    if (c == 'D')
    {
      //if it received the character it walks foward
      digitalWrite(LED_BUILTIN, HIGH);
      //delay(1500);
      for(int i = 0; i <= 100; ++i) leds.setPixelColor(i, B);
      leds.show();
      PORTD = ~((~PORTD)|(1<<leftPin1));//the command of type a = a|(1<<i) turns on the i-th bit in a
      PORTB = ~((~PORTB)|(1<<(rightPin1 - 8)));
      PORTB = ((PORTB)|(1<<(leftPin2 - 8))|(1<<(rightPin2 - 8)));//the command of type a = ~((~a)|(1<<i)) turns off the i-th bit in a
      delay(2500);
      PORTB = ~((~PORTB)|(1<<(leftPin2 - 8))|(1<<(rightPin2 - 8)));//the command of type a = ~((~a)|(1<<i)) turns off the i-th bit in a
      raiseFlag();
      for(int i = 0; i <= 100; ++i) leds.setPixelColor(i, R);
      leds.show();
    }
    else
    {
      
    }
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    
  }
}
