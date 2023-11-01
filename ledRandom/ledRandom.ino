byte x, y, z, a = 1;
byte rng() { 
    byte t = x ^ (x >> 1);
    x=y;
    y=z;
    z=a;
    a = z ^ t ^ ( z >> 3) ^ (t << 1);
    return a;
}
#define turnLed1(red, green, blue) PORTD = B00000000|(rng()<red)<<2|(rng()<green)<<3|(rng()<blue)<<4;

void setup() {
  pinMode(2, OUTPUT); //r
  pinMode(3, OUTPUT); //g
  pinMode(4, OUTPUT); //b
  cli();
  TCCR2A = 0;
  TCCR2B = 0; //we shall be using timer 2 (which is unused most of the time)
  TCCR2B |= (1<<CS20); //we set no prescaler (16MHz clock);
  const float tickTime = 1/(16e6);
  TIMSK2 |= (1<<OCIE2B); //we'll be using compare match B
  OCR2B = round(1e-5/tickTime); //we set it to go off every 0.01ms, ensuring averaged-out results
  sei(); //turning interrupts back on
  Serial.begin(115200);
}
void loop() {
  Serial.println("code not delayed!");
}
ISR(TIMER2_COMPB_vect){
    //when the interrupt triggers
    turnLed1(134, 12, 156); //arbitrary selection of colours (gives beautiful purple!!)
}
