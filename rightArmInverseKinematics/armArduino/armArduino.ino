#include <Servo.h>
#define innerRightShoulder 9
#define outerRightShoulder 8
#define rightElbow 7 //CAUTION! verify ports and change if needed
Servo irs, ors, re; //acronyms for above definitions
void setup() {
  // put your setup code here, to run once:
  irs.attach(innerRightShoulder);
  ors.attach(outerRightShoulder);
  re.attach(rightElbow);
  Serial.begin(115200);
  Serial.setTimeout(5); //usual set-up
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() && Serial.read() == 'S'){ //checks if it received the command to move the motors 
    byte a[3];
    for(int i = 0; i < 3; ++i){ //gets the angles from the pc and stores them in an array
      while(!Serial.available());
      a[i] = Serial.readString().toInt();
    }
    irs.write(a[0]);
    ors.write(a[1]); //sends them to the motors
    re.write(a[2]);
    //caution! you may need to put a timer afterwards.
  }
}
