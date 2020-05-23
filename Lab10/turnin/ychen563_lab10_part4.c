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

enum PWMStates { PWMStart, Wait, PWM_H, PWM_L }PWMState;
void PWMSM();

enum SetFrequencyStates { SetStart, WaitSet, Set, Hold } SetFrequencyState;
void SetFrequencySM();

unsigned char threeLEDs;
unsigned char blinkingLED;
unsigned char PWM;
unsigned long PWMPeriod = 1;



int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */


    const unsigned long timerPeriodGCD = 1;
    unsigned long BlinkLEDElapsedTime = 1000;
    unsigned long ThreeLEDsElapsedTime = 300;
    unsigned long CombineLEDElapsedTime = 1;
    unsigned long PWMElapsedTime = 1;
    unsigned long SetFrequencyElapsedTime = 100;

    TimerSet(timerPeriodGCD);
    TimerOn();

    BLState = BLStart;
    TLState = TLStart;
    CombineLEDState = OPStart;
    PWMState = PWMStart;
    SetFrequencyState = SetStart;

    while (1) {
      if (BlinkLEDElapsedTime >= 1000) {
         BlinkingLEDSM();
         BlinkLEDElapsedTime = 0;
      }
      if (ThreeLEDsElapsedTime >= 300) {
         ThreeLEDsSM();
         ThreeLEDsElapsedTime = 0;
      }
      if (CombineLEDElapsedTime >= 1) {
         CombineLEDsSM();
         CombineLEDElapsedTime = 0;
      }
      if (PWMElapsedTime >= PWMPeriod) {
         PWMSM();
         PWMElapsedTime = 0;
      }
      if (SetFrequencyElapsedTime >= 100) {
         SetFrequencySM();
         SetFrequencyElapsedTime = 0;
      }
      while (!TimerFlag){}
      TimerFlag = 0;
      BlinkLEDElapsedTime += timerPeriodGCD;
      ThreeLEDsElapsedTime += timerPeriodGCD;
      CombineLEDElapsedTime += timerPeriodGCD;
      PWMElapsedTime += timerPeriodGCD;
      SetFrequencyElapsedTime += timerPeriodGCD;
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

unsigned char tmpB;
void CombineLEDsSM() {
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
         tmpB = threeLEDs | (blinkingLED << 3) | (PWM << 4);
         break;
       default:
         break;
	}//State action

    PORTB = tmpB;
}


unsigned char L;
unsigned char H;
void SetFrequencySM() {
   unsigned char input = ~PINA & 0x03;
   switch (SetFrequencyState) {//Transitions
      case SetStart:
         SetFrequencyState = WaitSet;
         break;
      case WaitSet:
         if (input == 0x01 || input == 0x02){SetFrequencyState = Set;}
         else {SetFrequencyState = WaitSet;}
         break;
      case Set:
         SetFrequencyState = Hold;
      case Hold:
         if (input == 0x01 || input == 0x02){SetFrequencyState = Hold;}
         else {SetFrequencyState = WaitSet;}
         break;
      default:
         SetFrequencyState = SetStart;
         break;
   }//Transitions
   switch (SetFrequencyState) {//State Actions
      case SetStart:
         break;
      case WaitSet:
         break;
      case Set:
         if (input == 0x01 && (H > 1) && (L > 1)){H--;L--;}
         if (input == 0x02){H++;L++;}
         break;
      case Hold:
         break;
      default:
         SetFrequencyState = SetStart;
   }//State Actions
}


unsigned char i;
void PWMSM() {
   unsigned char input = ~PINA & 0x04;
   switch (PWMState) {//Transitions
      case PWMStart:
         PWMState = Wait;
         i = 0;
         L = 0x01;
         H = 0x01;
         break;
      case Wait:
         if (!input){PWMState = Wait;}
         else if (input){PWMState = PWM_H; i = 0;}
         break;
      case PWM_H:
         if (!input){PWMState = Wait;}
         else if (i <= H){PWMState = PWM_H;i++;}
         else if (i > H){PWMState = PWM_L; i = 0;}
         break;
      case PWM_L:
         if (i <= L){PWMState = PWM_L;i++;}
         else if (i > L){PWMState = PWM_H;}
         break;
      default:
         PWMState = PWMStart;
         break;
   }//Transitions
   switch (PWMState) {//State Action
      case PWMStart:
         break;
      case Wait:
         break;
      case PWM_H:
         PWM = 0x01;
         break;
      case PWM_L:
         PWM = 0x00;
         break;
      default:
         break;
   }//State Action
}

