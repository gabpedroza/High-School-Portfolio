#include "SoftwareSerial.h"
#define initial_angle 125
#define head_pin 7
#define leftDCPin1 40
#define rightDCPin1 41
#define leftDCPin2 44
#define rightDCPin2 45
#define leftDCPwm 42
#define rightDCPwm 43
// RX: Arduino pin 47, XBee pin DOUT.  TX:  Arduino pin 49, XBee pin DIN
SoftwareSerial XBee(47, 49);
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#define K 0x000000//BLACK
#define B 0x0048ff//BLUE
#define G 0x008000//GREEN
#define P 0x9d00d2//PURPLE
#define R 0xFF0000//RED
#define O 0xFFA500//ORANGE
#define S 0xFFC0CB//ROSE
#define Y 0xFf6400//YELLOW
#define W 0xFFFFFF//WHITE
#define PIN 12// Pin of controle
#define LED_COUNT 66// Number of leds
//Cria uma instância da Classe Adafruit NeoPixel chamado "leds"
#define innerLeftShoulder 8
#define outerLeftShoulder 10
#define leftElbow 9
#define innerRightShoulder 2
#define outerRightShoulder 3
#define rightElbow 5//CAUTION! verify ports and change if needed
class articulation{  
  public:
    articulation(Servo mmotor, int ini, int(*ffunc)(int x)){ //initializes an articulation
      _motor = mmotor; //motor
      _func = ffunc; //function to adjust angles
      _ang = _func(ini); //angle in which the articulation is
      move(ini); //sets the initial position
    }
    void move(int x, int del){
      x = _func(x); //adjusts angle
      if(x > _ang){ //if it needs to increase angle
        for(; _ang <= x; ++_ang){
          _motor.write(_ang);
          delay(del); //moves slowly
          //Serial.println("subindo" + String(_ang));
        }
        _ang=x; //sets the end angle to desired
      }else{
        for(; _ang >= x; --_ang){
          _motor.write(_ang);
          delay(del);//moves slowly
          //Serial.println("descendo" + String(_ang));
        }
        _ang=x; //sets the end angle to desired
      }
    }
    void move(int x){
      move(x, 15); //quick move, with standard speed
    }
    void brute(int x){
      _motor.write(x); //write directly to motor
    }
  private:
    int _ang;
    Servo _motor;
    int(*_func)(int x);
};
Servo mirs, mors, mre, mils, mols, mle; //acronyms for above definitions
articulation *irs, *ors, *re, *ils, *ols, *le; //articulations
//functions for adjusting angle 
Servo head_motor;

int adjustIRS(int x){ return x;}
int adjustORS(int x){ return 180-x;}
int adjustRE(int x){ return (x>155)?180-155:180-x;}
int adjustILS(int x){ return 180-x;}
int adjustOLS(int x){ return 180-x;}
int adjustLE(int x){ return (x>155)?180-155:180-x;}
void greet();
void selfDestroy();
void calculate();
void track();
void receipt();
void angrier();
void normal();
void understand();
void translate();
void object();
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

//unsigned long time;//Will count the time
//unsigned long timeout;//timeout
int brilho[3];//Receives the values of the serial
//int i=0;//amount of characters
int posicao=0;
int flag=0;//Informs that a information by the serial was received
char temp,cor;//Storages data received by the serial
void setup() {
  // put your setup code here, to run once:
  head_motor.attach(head_pin);
  head_motor.write(initial_angle); 
  leds.begin();
  //leds.setPixelColor(30, B);
  leds.show();
  pinMode(13, OUTPUT);
  //digitalWrite(6, 1);
  pinMode(leftDCPin1, OUTPUT);
  pinMode(leftDCPin2, OUTPUT);
  pinMode(rightDCPin1, OUTPUT);
  pinMode(rightDCPin2, OUTPUT);
  pinMode(rightDCPwm, OUTPUT);
  pinMode(leftDCPwm, OUTPUT);
  mirs.attach(innerRightShoulder);
  mors.attach(outerRightShoulder);
  mre.attach(rightElbow);
  mils.attach(innerLeftShoulder);
  mols.attach(outerLeftShoulder);
  mle.attach(leftElbow);
  delay(5000);
  irs = new articulation(mirs, 0, adjustIRS);
  delay(2000);
  ors = new articulation(mors, 0, adjustORS);
  delay(2000);
  re = new articulation(mre, 0, adjustRE);
  delay(2000);
  ils = new articulation(mils, 0, adjustILS);
  delay(2000);
  ols = new articulation(mols, 0, adjustOLS);
  delay(2000);
  le = new articulation(mle, 0, adjustLE);
  delay(12000);//initializing motors. plenty of delay to let capacitors stabilize
  Serial.begin(115200);
  delay(200);
  Serial.setTimeout(5); //usual set-up
  XBee.begin(9600);
  leds.setPixelColor(10, R);
  leds.show();
  //Serial.println("ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    //Serial.println("opa");
    char i = Serial.read();
    if(i == 'D'){
      //makes her angry and sends the little kart
      selfDestroy();
    }else if(i == 'G'){
      //waves goodbye and turns on the leds
      greet();
    }else if(i == 'T'){ //tracking
      //recognize
      understand();
      track();
      normal();
    }else if(i == 'O' || i == 'C'){ //ocr or calculate
      //recognize
      understand();
      normal();
    }else if(i == 'R'){ //receipt
      //recognize kinda angry
      receipt();
    }else if(i == 'A'){
      //recognize even angrier
      angrier();
    }else if(i == 'I'){
      object();
      digitalWrite(6, 1);
    }
    Serial.flush();
  }
}
void angrier(){
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%3 == 0){
        leds.setPixelColor(i,R);//turns the leds mutiples of three red
        delay(10);
        leds.show();
      }
    }
    for(int x = 0; x <=250;){//makes the go full circle aproximately three times
      for (int i = 0; i<=LED_COUNT; i++)
      {
     
        leds.setPixelColor(i,Y);//turns the led yellow
        delay(5);
        if((i-3)%3==0 || (i-3)%5==0 || (i-3)%7==0){//if the former was red, it returns blue
        leds.setPixelColor(i-3, R);
        leds.show();
        delay(10);
      }
       else{//if it wasn't red (was blue) it returns blue
        leds.setPixelColor(i-3, B);
        leds.show();
        delay(10);
       }
       if (i%6==0){//if its multiple of six, becomes yellow
        leds.setPixelColor(i-3, Y);
        leds.show();
        delay(10);
       }
      x = x+1;
     }
    }
}
void selfDestroy(){
    digitalWrite(LED_BUILTIN, HIGH);
  //XBee.write('H');//orders the secondary robot to leave
  bool a = true;
  for(int x = 0; x > -160 && a;){
    for (int i = 0; i<=LED_COUNT &&a; i++)//makes an yellow circle
    {
      leds.setPixelColor(i,Y);//activates the required led
      delay(20);
      leds.setPixelColor(i-10, R);
      leds.show();
      x++;
      if(x >= 50*5.8 && a){ XBee.write('H'); a = false;}//orders the secondary robot to leave
    }
  }
  delay(2000);
  understand();
}
void greet(){
  for (int i = 0; i<=LED_COUNT; i++)
    {
      leds.setPixelColor(i,P);//Turns the color
      delay(7);
      //leds.setPixelColor(i-15, P);
      leds.show();
    }
    delay(100);
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%7 == 0){
        leds.setPixelColor(i,B);//turns the leds mutiples of seven blue
        delay(15);
        leds.show();
      }
    }
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%5 == 0){
        leds.setPixelColor(i,B);//turns the leds mutiples of five blue
        delay(15);
        leds.show();
      }
    }
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%2 == 0){
        leds.setPixelColor(i,B);//turns the leds mutiples of 2 blue
        delay(15);
        leds.show();
      }
    }
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%1 == 0){
        leds.setPixelColor(i,B);//turns all leds blue
        delay(15);
        leds.show();
      }
    }
    ors->move(50, 5);
    re->move(100, 5);
    irs->move(180, 5);
    delay(1000);
    re->move(30, 5);
    re->move(100, 5);
    delay(1000);
    irs->move(0, 5);
    re->move(0, 10);
    ors->move(0, 10);
 
}
void understand(){
    for(int x = 0; x<=300;){
      for (int i = 0; i<=LED_COUNT; i++){
        leds.setPixelColor(i,G);//activates the required led
        delay(10);
        leds.setPixelColor(i-15, B);
        leds.show();
        x = x+1;
      }
   }
}
void track(){
  int angulo = initial_angle;
  /*leds.setPixelColor(40, R);
  leds.show();
  delay(300);
  leds.setPixelColor(40, B);
  delay(300);
  leds.show();*/
  int lista[6]; int j = 0;
  long int timee = millis();
  while(millis() - timee <= 4000 || true){
    /*leds.setPixelColor(30, Y);
    leds.show();
    delay(100);
    leds.setPixelColor(30, B);
    leds.show();
    delay(100);*/
    if(Serial.available()){
      /*leds.setPixelColor(40, R);
      leds.show();
      delay(100);
      leds.setPixelColor(40, B);
            leds.show();
      delay(100);*/

      ++j;
      int i = Serial.readString().toInt(); //here, we get our displacement from the PC
      if(abs(i-10)<=1) i = 10;//if the displacement is too small, we just ignore it
      lista[j] = -i+10; //since we cannot send negative numbers over serial, we have to convert the displacement to a proper scale
      /*
       * j, list[], maxx, minn, and total are all auxiliary variables that help us make our angles consistent
       * 
       * ocasionally, due to fluctuations, the Python code will send angles that are not coherent with reality
       * thus, in order to filter these outliers out, we store 4 successive values in a vector before writing the angle to the motor
       * we then disregard both the highest and the lowest values. we average the two left out, and only then do we send the angle to the motor
       * 
       * notice that this process takes time, which requires us to lower the explicit delay associated with the movement
       * (since there's a lot of time involved in doing these calculations, waiting for the receipt of four values, etc
       * that the motor will already utilize in its movement)
       */
      if(j == 4){
        int maxx = 1, minn = 1, total = 0; 
        for(int k = 1; k <= 4; ++k){
          if(lista[k] > lista[maxx]) maxx = k;
          if(lista[k] < lista[minn]) minn = k;
          total += lista[k];
        }
        angulo += 1*(total-lista[maxx]-lista[minn])/2;
        if(angulo < 0) angulo = 0;
        else if(angulo > 180) angulo = 180;
        head_motor.write(angulo);
        digitalWrite(LED_BUILTIN, 1);
        j = 0; //of course, it is necessary to reset the index of the list after sending the angle
        //delay(5);
        digitalWrite(LED_BUILTIN, 0);
      }
    }
    Serial.flush();//we flush it to be certain that the values read are always up-to-date 
  }
}
void normal(){
    for (int i = 0; i<=LED_COUNT; i++)
    {
      leds.setPixelColor(i,B);//activates the required led
      delay(20);
      leds.setPixelColor(i-10, B);
      leds.show();
    }
}
void receipt(){
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%5 == 0){
        leds.setPixelColor(i,R);//turns the leds mutiples of five red
        delay(15);
        leds.show();
      }
    }
    for(int i = 0; i <= LED_COUNT; ++i){
      if(i%7 == 0){
        leds.setPixelColor(i,R);//turns the leds mutiples of seven red
        delay(15);
        leds.show();
      }
    }  
}
void translate(){
  //move motors
}
void object(){
  ors->move(50, 5);
  re->move(100, 5);
  irs->move(180, 5);
  delay(500);
  re->move(30, 5);
  int angle = 30;
  long int timee = millis();
  while(millis() - timee <= 4000||true){
    if(Serial.available()){
      int i = Serial.readString().toInt(); //here, we get our displacement from the PC
      if(abs(i-10)<=1) i = 10;//if the displacement is too small, we just ignore it
      angle += i;
      if(angle < 0) angle = 0;
      else if(angle > 180) angle = 180;
      re->move(angle);
      digitalWrite(LED_BUILTIN, 1);
      //delay(5);
      digitalWrite(LED_BUILTIN, 0);
    }
    Serial.flush();//we flush it to be certain that the values read are always up-to-date 
  }
}
