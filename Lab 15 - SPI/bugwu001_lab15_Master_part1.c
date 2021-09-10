/*	Author: Buddy Ugwumba
 *  Partner(s) Name: None
 *	Lab Section: 021
 *	Assignment: Lab #15 Exercise # 1
 *	Exercise Description: [optional - include for your own benefit]
 * 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Buddy Ugwumba
 *  Demo Link: Youtube URL> 
 *  https://drive.google.com/file/d/1ctpIlPbeXbFgPoAn2WG3pxMbz9Lhl8uL/view?usp=sharing
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "keypad.h"
#include "bit.h"
#include "scheduler.h"
#include "timer.h"
#include "SPI.h"

static unsigned char value = 0x00;
enum Incrementor {Start, Increment};

int Increment_SMTick (int state) {

    switch (state) { //State Transitions
        case Start:
            state = Increment;
            break;
        case Increment:
            state = Increment;
            break;
        default:
            break;
    }

    switch (state) { //State Actions
        case Start:
            break;
        case Increment:
            if(value == 0xFF){
                value = 0x00;
            }
            SPI_MasterTransmit(value);
            value++;
            break;
        default:
            break;
    }

    return state;
}

int main (void){

    DDRB = 0xFF; PORTB = 0x00;

    //Declare an arry of tasks
    static task task1;
    task *tasks[] = { &task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    //Task 1 checks Keypad for new input
    task1.state = Start;//Task initial state.
    task1.period = 1000;//Task Period
    task1.elapsedTime = task1.period;//Task current elapsed time
    task1.TickFct = &Increment_SMTick;

    unsigned long GCD = tasks[0]->period;
    for(unsigned short i = 1; i < numTasks; i++){
        GCD = findGCD(GCD, tasks[i]->period);
    }

    //Set the timer and turn it on
    TimerSet(GCD);
    TimerOn();
    SPI_MasterInit();
    while(1) {
        for (unsigned char i = 0; i < numTasks; i++){ //Scheudler code
            if(tasks[i]->elapsedTime == tasks[i]->period) { //Task is reasdy to tick
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0; //Reset the elapsed time for next tick

            }
            tasks[i]->elapsedTime += GCD;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0; // Error: program should not exit

}