#include<Servo.h>
#define initial_angle  90
#define head_pin 6
Servo head_motor;

void setup() {
   head_motor.attach(head_pin);
   head_motor.write(initial_angle); 
   pinMode(13, OUTPUT);
   Serial.begin(115200);
   delay(200); //up to this line it is a normal initialization
   Serial.setTimeout(5); //here, we set the Serial to work much, much faster than usual, as is necessary for smooth tracking
}
int angle = initial_angle;
int list[6]; int j = 0;
void loop(){
  if(Serial.available()){
    ++j;
    int i = Serial.readString().toInt(); //here, we get our displacement from the PC
    if(abs(i-10)<=1) i = 10;//if the displacement is too small, we just ignore it
    list[j] = -i+10; //since we cannot send negative numbers over serial, we have to convert the displacement to a proper scale
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
      int maxx = 0, minn = 0, total = 0;
      for(int k = 1; k <= 4; ++k){
        if(list[k] > list[maxx]) maxx = k;
        if(list[k] < list[minn]) minn = k;
        total += list[k];
      }
      angle += (total-list[maxx]-list[minn])/2;
      head_motor.write(angle);
      j = 0; //of course, it is necessary to reset the index of the list after sending the angle
      delay(5);
    }
  }
  Serial.flush();//we flush it to be certain that the values read are always up-to-date 
}
