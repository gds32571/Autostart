/**************************************************
     AutoStart
     Purpose: to restart an UPS3V2 one hour after shutdown
              due to lightning detected.
     Uses sleep mode to minimize power consumption
     Powered by 2xAA cells
     
    10 June 2019 - gswann

  Adafruit cable
  Red   +5 volts from USB port
  White receive (to laptop) at 3.3v level
  Green transmit (from laptop) at 3.3v level
  Black ground

  Don't connect green wire if doing low power sleep functions.

  11 June 2019 - gswann
  Built prototype PCB today. I goofed up the blinky LED connection,
  high is off, low is on.
  Thus the #define and the #ifdef below

  29 Aug 2019 - gswann
  changing so that it keeps trying if power is off 
  more than an hour.
  Like if a hurricane happens!

**************************************************/

// pick one
// prototype board built 11 June 2019
// #define PROTOBOARD
// Oshpark - nice board
#define PCB

#include <avr/sleep.h>
#include <avr/wdt.h> //Needed to enable/disable watch dog timer

#include <SoftwareSerial.h>

// Utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

// Definitions
#define rxPin 4        // package pin 3
#define txPin 3        // package pin 2
#define ledPin 1
#define systemStart 0  // package pin 5

SoftwareSerial mySerial(rxPin, txPin);

int sensorPin = A1; //ACTUALLY PIN LABELED AS "2" on the HLT tutorial
int sensorVal = -1;
int ledState = LOW;
int buzzerState = LOW;
int watchdog_counter = 0;

// about 90 seconds
// int startAfter = 10;

// about 60 minutes
int startAfter = 450;

void setup_watchdog(int);

//This runs each time the watch dog wakes us up from sleep
ISR(WDT_vect) {
  watchdog_counter++;
}

#ifdef PROTOBOARD
const bool myOn = LOW;
const bool myOff = HIGH;
#endif

#ifdef PCB
const bool myOn = HIGH;
const bool myOff = LOW;
#endif

void setup() {
  delay(100);

  mySerial.begin(300);
  mySerial.print("Startup controller starting...");
  mySerial.println();

  digitalWrite(systemStart, LOW);
  pinMode(systemStart, OUTPUT);

  // blink once at startup
  digitalWrite(ledPin, myOff);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, myOn);
  delay(2000);
  digitalWrite(ledPin, myOff);
  delay(1000);

  // blink again (twice) at startup for long periods
  if (startAfter >= 450) {
    digitalWrite(ledPin, myOn);
    delay(2000);
    digitalWrite(ledPin, myOff);
  }


  //  adc_disable(); // ADC uses ~320uA
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void enterSleep(void)
{
  sleep_enable();
  sleep_cpu();
}

// the loop routine runs over and over
void loop() {

  //      ADCSRA &= ~(1 << ADEN); //Disable ADC, saves ~230uA
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

  setup_watchdog(9); //Setup watchdog to go off after 8 sec
  //sleep_mode(); //Go to sleep! Wake up 1sec later and check water

  // pinMode(ledPin, INPUT);
  adc_disable();
  enterSleep();

  // back after eight seconds

  digitalWrite(ledPin, myOn);
  delay(30);
  digitalWrite(ledPin, myOff);
  if (watchdog_counter > (startAfter - 5)) {
    delay(80);
    digitalWrite(ledPin, myOn);
    delay(30);
    digitalWrite(ledPin, myOff);
  }


  if (watchdog_counter > startAfter) {
    mySerial.println("Restarting system...");

    //    digitalWrite(ledPin, LOW);
    //    delay(50);

    digitalWrite(systemStart, HIGH);
    enterSleep();
    delay(7000);
    digitalWrite(systemStart, LOW);

    // this should restart the one hour delay
    // if restart fails
    watchdog_counter = 0;

//    while (1) {
//      enterSleep();
//    }
  }

}  // loop


void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
