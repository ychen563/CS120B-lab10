/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif


enum BLStates { BLStart, BL_s1 }BLState;
void BlinkingLEDSM();

enum TLStates { TLStart, TL_s1, TL_s2, TL_s3 }TLState;
void ThreeLEDsSM();

enum CombineLEDStates { OPStart, Output }CombineLEDState;
void CombineLEDsSM();

unsigned char threeLEDs;
unsigned char blinkingLED;



int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */


    const unsigned long timerPeriodGCD = 100;
    unsigned long BlinkLEDElapsedTime = 1000;
    unsigned long ThreeLEDsElapsedTime = 1000;
    unsigned long CombineLEDElapsedTime = 1000;

    TimerSet(timerPeriodGCD);
    TimerOn();

    BLState = BLStart;
    TLState = TLStart;
    CombineLEDState = OPStart;

    while (1) {
      if (BlinkLEDElapsedTime >= 1000) {
         BlinkingLEDSM();
         BlinkLEDElapsedTime = 0;
      }
      if (ThreeLEDsElapsedTime >= 1000) {
         ThreeLEDsSM();
         ThreeLEDsElapsedTime = 0;
      }
      if (CombineLEDElapsedTime >= 1000) {
         CombineLEDsSM();
         CombineLEDElapsedTime = 0;
      }
      while (!TimerFlag){}
      TimerFlag = 0;
      BlinkLEDElapsedTime += timerPeriodGCD;
      ThreeLEDsElapsedTime += timerPeriodGCD;
      CombineLEDElapsedTime += timerPeriodGCD;
    }
    return 1;
}

void BlinkingLEDSM() {
  switch(BLState) { // Transitions
     case BLStart: // Initial transition
        blinkingLED = 0; // Initialization behavior
        BLState = BL_s1;
        break;
     case BL_s1:
        BLState = BL_s1;
        break;
     default:
        BLState = BLStart;
        break;
   } // Transitions

  switch(BLState) { // State actions
     case BLStart:
        break;
     case BL_s1:
        if (blinkingLED == 0){blinkingLED = 1;}
        else if (blinkingLED == 1){blinkingLED = 0;}
        break;
     default:
        break;
  } // State actions
}

void ThreeLEDsSM() {
   switch(TLState) { // Transitions
     case TLStart: // Initial transition
        TLState = TL_s1;
        break;
     case TL_s1:
        TLState = TL_s2;
        break;
     case TL_s2:
        TLState = TL_s3;
        break;
     case TL_s3:
        TLState = TL_s1;
        break;
     default:
        TLState = TLStart;
   } // Transitions

  switch(TLState) { // State actions
     case TL_s1:
        threeLEDs = 0x01;
        break;
     case TL_s2:
        threeLEDs = 0x02;
        break;
     case TL_s3:
        threeLEDs = 0x04;
        break;
     default:
        break;
  } // State actions
}

void CombineLEDsSM() {
   unsigned char tmpB;
   switch (CombineLEDState) { //Transitions
      case OPStart:
         CombineLEDState = Output;
         break;
      case Output:
         CombineLEDState = Output;
         break;
      default:
         CombineLEDState = OPStart;
    }//Transition

    switch (CombineLEDState) {//State action
       case OPStart:
         break;
       case Output:
         tmpB = threeLEDs | (blinkingLED << 3);
         break;
       default:
         break;
	}//State action

    PORTB = tmpB;
} 

