#include <Servo.h>
#define innerRightShoulder 2
#define outerRightShoulder 3
#define rightElbow 5 //CAUTION! verify ports and change if needed
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
Servo mirs, mors, mre; //acronyms for above definitions
articulation *irs, *ors, *re; //articulations
//functions for adjusting angle 
int adjustIRS(int x){ return x;}
int adjustORS(int x){ return 180-x;}
int adjustRE(int x){ return (x>155)?180-155:180-x;}
void setup() {
  // put your setup code here, to run once:
  mirs.attach(innerRightShoulder);
  mors.attach(outerRightShoulder);
  mre.attach(rightElbow);
  delay(5000);
  irs = new articulation(mirs, 0, adjustIRS);
  delay(2000);
  ors = new articulation(mors, 0, adjustORS);
  delay(2000);
  re = new articulation(mre, 0, adjustRE);
  delay(12000);//initializing motors. plenty of delay to let capacitors stabilize
  Serial.begin(115200);
  Serial.setTimeout(5); //usual set-up
}

void loop() {
  // put your main code here, to run repeatedly:
}
