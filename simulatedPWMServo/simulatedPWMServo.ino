/*
   This code is able to simulate a servo-ready PWM signal in theoretically any digital port.
   The only downside is that buzzers will stop working with the tone() function :(
   -----------------------------------------------------------------------------------------
   SUMMARY:
   Servos check for inputs at a frequency of 50Hz. The arduino must send a pulse to it
   whose width will be translated into an angle in the servo.
   However, the pulse length must be very accurate. For every 1/50Hz = 20ms interval the servo checks for a signal,
   an ON width between minTime seconds and maxTime seconds will be enough to encapsulate the whole 0-180 range.
   Thus, this code attempts to send such a pulse periodically through the use of interrupts.

   First, it breaks down the 20ms interval into several smaller blocks of length (maxTime - minTime) / 180,
   that shall be called wave units.

   Second, it sets up Arduino's timer2 to go off every waveUnitTime seconds,
   i.e. each of these smaller blocks corresponds to an interrupt. The value (maxTime - minTime) / 180 was
   chosen because it ensures enough resolution to reach any angle (1deg resolution).

   Finally, inside the interrupt the Arduino decides if that specific portion of the 20ms interval
   should correspond to an ON or OFF moment, thus emulating the PWM wave.
   -----------------------------------------------------------------------------------------
   APPENDICE ONE: TIMERS
   I chose timer2 because I was going to mess one timer anyway and timer2 was useless (sorry buzzer).
   This arduino timer works by increasing in one unit every clock cycle. After it reaches 255, it
   collapses back to 0 and restarts counting.

   Because this corresponds to an increase every tick = 62.5ns, I needed to set the timer to interrupt the routine
   every time it reached the value of waveUnitTime/tick, i.e. every wave unit. In the interrupt, I restart it to 0.

   Thus, the timer is essentially trapped in an infinite cycle of triggering every wave unit.
   Each time it triggers, the code stops whatever the hell it is doing and jumps to the ISP function.
   After completing it (NB! the timer still ticks inside it), the code rollsback to wherever it was.

   You can learn more about timers at the following illuminating article:
   https://electronoobs.com/eng_arduino_tut140.php
   -----------------------------------------------------------------------------------------
   APPENDICE TWO: INSIDE THE INTERRUPT
   Of course, interrupting the code every 10us or so doesn't cut it on its own.

   The most important idea of the code is that by breaking the whole 20ms interval into tiny wave units,
   we are able to pretend each wave unit is signal on its own.
   Hence, if we come up with a way to decide when the signal is ON and when it is OFF, we are done with the problem.

   The way I faced this was by means of a very simple universal counter counting how many. interrupts (=wave units) already happened.
   We know that, in terms of wave units, a whole PWM period lasts totalWaveUnits, so every time the count crosses this value,
   we simply reset it.

   Furthermore, we can imagine this counter as enumerating each wave unit that goes by, much like how each year
   has their own unique number. Then, it suffices to keep the digital port ON as long as the "year" doesn't surpass
   the pulse width we are trying to achieve, i.e. as long as count is below want.

   Then there comes a bit of math: its is easy to see, through a simple map function, that one can easily translate
   the 0-180 range into a specified interval in the minTime-maxTime range (think of temperature scales!).
   We also have a way to translate a time interval into a number of wave units (function waveUnit()).
   Thus, we simply do both of these translations simultaneously to arrive at a macro (aTc(x)) that translates
   angles into counts of wave units (NB! macros are simillar to functions, but can get messy if weird expressions are inside them).

   Therefore, in the end, we simply check if the current wave unit number is higher than the specified number of wave units that should be ON.
   If it is, then it turns OFF. Otherwise, ON it is.
   -----------------------------------------------------------------------------------------
   TRICKS
   I used direct port register because the interrupt triggers something like every 150 clock cycles.
   The code inside the interrupt call may be short, but it has to do a lot of things (increase counter, reset timer, check ifs, switch ports...)
   Each of these things costs AT LEAST a full clock cycle. So before you know this code i already eating processing power.
   Of course, y6our code will continue to run normally etc. But these precious clock cycles that could be speeding up your routine
   will be eaten by the nasty PWM simulation.

   Anyway, port registers ensure that I can save as many clock cycles as possible (same reason for ++count and not count++)

   Also, don't ask me about why a chose aTc to be a macro; it made sense because I was saving a function call instruction, 
   but now I realise it is outside the ISP interrupt :/
*/

#define port 2 //your servo port
double minTime = 300e-6; //the pulse width corresponding to 0deg. may vary
double maxTime = 2000e-6; //the pulse width corresponding to 180deg. may vary
double tick = 62.5e-9; //your arduino clock period
double waveUnitTime = (maxTime - minTime) / 180; //time length of each wave unit
int waveUnit(double ttime) { //function that converts time length to number of wave units
  return round(ttime / waveUnitTime);
}
int minWaveUnits = waveUnit(minTime);
int maxWaveUnits = waveUnit(maxTime); //blahblah
int totalWaveUnits = waveUnit(20e-3);
#define aTc(x) (( x*(maxWaveUnits - minWaveUnits) / 180 ) + minWaveUnits) //convert angle to number of wave units in ON state
int count = 0; //counter of number of interrupts=wave units
int want = aTc(0); //the number of wave units corresponding to your angle. just changing it changes the servo angle
//weird, I know, but the interrupt will eventually happen and notice your change.
void setup() {
  pinMode(port, OUTPUT); //bleh
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2B |= (1 << CS20);
  TIMSK2 |= (1 << OCIE2B);
  OCR2B = round(waveUnitTime / tick);
  sei(); //setting up timer. no prescaler, compare match mode etc.
}
void loop() {
  //your code here :D
  //write as you wish (this is a sample sweep code)
  for (int pos = 0; pos <= 180; ++pos) {
    want = aTc(pos);
    delay(5);
  }
  for (int pos = 180; pos >= 0; --pos) {
    want = aTc(pos);
    delay(5);
  }
}

ISR(TIMER2_COMPB_vect) { //the INTERRUPT!!1!!!
  TCNT2 = 0; //zeroes the timer. better if put at the start, because putting it in the end will mess the count
  //i.e. the time the Arduino spends doing this function should be taken into account for next interrupts
  ++count; //yay another interrupt
  if (count >= totalWaveUnits) count = 0; //explained in the appendices
  if (count >= want) {
    PORTD = ~((~PORTD) | (1 << port)); //"explained" in the tricks
  } else {
    PORTD = PORTD | (1 << port);
  }
}
