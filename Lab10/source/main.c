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

typedef struct task {
  int state; // Current state of the task
  unsigned long period; // Rate at which the task should tick
  unsigned long elapsedTime; // Time since task's previous tick
  int (*Tick)(int); // Function to call for task's tick
} task;

task tasks[3];

const unsigned char tasksNum = 3;
const unsigned long tasksPeriodGCD = 1000;
const unsigned long periodBlinkLED = 1000;
const unsigned long periodThreeLEDs = 1000;
const unsigned long periodOutput = 1000;

enum BLStates { BLStart, BL_s1 };
int BlinkingLEDSM(int state);

enum TLStates { TLStart, TL_s1, TL_s2, TL_s3 };
int ThreeLEDsSM(int state);

enum CombineLEDStates { OPStart, output };
int CombineLEDsSM(int state);

unsigned char threeLEDs;
unsigned char blinkingLED;

void TimerISR() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].Tick(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */

    unsigned char i=0;
    tasks[i].state = BLStart;
    tasks[i].period = periodBlinkLED;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &BlinkingLEDSM;
    ++i;
    tasks[i].state = TLStart;
    tasks[i].period = periodThreeLEDs;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &ThreeLEDsSM;
    ++i;
    tasks[i].state = OPStart;
    tasks[i].period = periodOutput;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Output;

    TimerSet(tasksPeriodGCD);
    TimerOn();

    while (1) {}
    return 1;
}

int BlinkingLEDSM(int state) {
  switch(state) { // Transitions
     case BLStart: // Initial transition
        blinkingLED = 0; // Initialization behavior
        state = BL_s1;
        break;
     case BL_s1:
        state = BL_s1;
        break;
     default:
        state = BLStart;
   } // Transitions

  switch(state) { // State actions
     case BL_s1:
        if (blinkingLED == 0){blinkingLED = 1;}
        else {blinkingLED = 0;}
        break;
     default:
        break;
  } // State actions
  return state;
}

int ThreeLEDsSM(int state) {
   switch(state) { // Transitions
     case TLStart: // Initial transition
        state = TL_s1;
        break;
     case TL_s1:
        state = TL_s2;
        break;
     case TL_s2:
        state = TL_s3;
        break;
     case TL_s3:
        state = TL_s1;
        break;
     default:
        state = TLStart;
   } // Transitions

  switch(state) { // State actions
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
  return state;
}

int CombineLEDsSM(int state) {
   switch (state) { //Transitions
      case OPStart:
         state = Output;
         break;
      case Output:
         state - Output;
         break;
      default:
         state - OPStart;
    }//Transition

    switch (state) {//State action
       case OPStart:
         break;
       case Output:
         PORTB = threeLEDs | (blinkingLED << 3);
         break;
       default:
         break;
	}//State action
} 

