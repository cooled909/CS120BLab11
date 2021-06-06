/*	Author: Edward Segura
 *  Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab 11  Advancement 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *  Demo Link: https://drive.google.com/file/d/1eVghVSSJtNpH2P35nj3OWta9NUWKCnbA/view?usp=sharing
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../header/keypad.h"
#include "../header/timer.h"
#include "../header/scheduler.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

/*Basic Requirement Checklist:
*
*Main - Player Paddle Control - Done
*Main - Ball Physics (3 steps) -Done
*Main - Draw state - Done
*Main - Ball and Paddle interaction - Done
*Main - AI Paddle engine - Done
*Main - Reset Button - Done
*/


enum P1_Movements {P1_start, P1_Wait, P1_Up, P1_Down, P1_Hold};
unsigned char key;
static unsigned char paddlePattern = 0x80;
static unsigned char paddleRow = 0xF3;
int MoveTick(int state){
    key = GetKeypadKey();
    switch(state){
        case P1_start:
        state = P1_Wait;
        break;
        case P1_Wait:
        if(key == 'B'){
            state = P1_Up;
        }
        if(key == 'A'){
            state = P1_Down;
        }
        break;
        case P1_Up:
            state = P1_Hold;
        break;
        case P1_Down:
        state = P1_Hold;
        break;
        case P1_Hold:
        if(key == '\0'){
            state = P1_Wait;
        }
        break;
        default:
        state = P1_start;
        break;
    }
    switch(state){
        case P1_start:
        break;
        case P1_Up:
        if(paddleRow != 0xFC){
            paddleRow = (paddleRow >> 1) | 0x80;
        }
        break;
        case P1_Down:
        if(paddleRow != 0xE7){
            paddleRow = (paddleRow << 1) | 0x01;
        }
        break;
        case P1_Hold:
        break;
    }

    return state;
}

static unsigned char CPUpaddlePattern = 0x01;
static unsigned char CPUpaddleRow = 0xF3;


int DetectHit(unsigned char ballRow, unsigned char paddleRow, unsigned char yDirection){
    unsigned char ballHit = ~(ballRow) & ~(paddleRow);
    unsigned char nextBallHitY1 = ~((ballRow >> 1) | 0x80) & ~(paddleRow);
    unsigned char nextBallHitY0 = ~((ballRow << 1) | 0x01) & ~(paddleRow);
    //returns 0 if no shot, 1 if center shot, 2 if corner shot
    if(ballHit != 0x00){
        return 1;
    }
    if(yDirection == 2 && (nextBallHitY0 != 0x00)){
        return 2;
    }
    if(yDirection == 1 && (nextBallHitY1 != 0x00)){
        return 2;
    }
    return 0;
}

//--------------------------------------
// LED Matrix Demo SynchSM
// Period: 100 ms
//--------------------------------------
enum Demo_States {shift};
static unsigned char ballPattern = 0x08;    // LED pattern - 0: LED off; 1: LED on
static unsigned char ballRow = 0xEF;      // Row(s) displaying pattern.
static unsigned char xDirection = 0x01; //1 = positiveX, 0 = negativeX
static unsigned char yDirection = 0x01; //0 = nothingY, 1 = positiveY, 2 = negativeY
static unsigned long ballSpeed = 200;
int Demo_Tick(int state) {
    key = GetKeypadKey();

    // Local Variables
 
                            // 0: display pattern on row
                            // 1: do NOT display pattern on row 
    // Transitions
    switch (state) {
        case shift:    
break;
        default:    
state = shift;
            break;
    }    
    // Actions
    switch (state) {
    case shift:    
    if (ballRow == 0xFE) {  
        yDirection = 0x02;
    } 
    if (ballPattern == 0x02 && DetectHit(ballRow, CPUpaddleRow, yDirection) == 1)  { 
        xDirection = 0x00;
        ballSpeed = 200;
    }
    else if (ballPattern == 0x02 && DetectHit(ballRow, CPUpaddleRow, yDirection) == 2)  { 
        if(yDirection == 0x02){
            yDirection = 0x01;
            xDirection = 0x00;
        }
        else if(yDirection == 0x01){
            yDirection = 0x02;
            xDirection = 0x00;
        }
        ballSpeed = 125;
    }
    if (ballRow == 0xEF) {  
        yDirection = 0x01;
    } 
    if (ballPattern == 0x40 && DetectHit(ballRow, paddleRow, yDirection) == 1)  { 
        xDirection = 0x01;
        ballSpeed = 200;
    }
    else if (ballPattern == 0x40 && DetectHit(ballRow, paddleRow, yDirection) == 2)  { 
        if(yDirection == 0x02){
            yDirection = 0x01;
            xDirection = 0x01;
        }
        else if(yDirection == 0x01){
            yDirection = 0x02;
            xDirection = 0x01;
        }
        ballSpeed = 125;
    }
    if(xDirection == 0x01){
        ballPattern = ballPattern >> 1;
    }
    else{
        ballPattern = ballPattern << 1;
    }
    if(yDirection == 0x01){
        ballRow = (ballRow >> 1) | 0x80;
    } 
    else{
        ballRow = (ballRow << 1) | 0x01;
    }
    if(key == '5'){
        ballPattern = 0x08;    // LED pattern - 0: LED off; 1: LED on
        ballRow = 0xEF;
        xDirection = 1;
        yDirection = 1;
    } 
    break;
    default:
    break;
    }
        // Row(s) displaying pattern    
    return state;
}
enum CPU_Movements {CPU_start, CPU_Wait, CPU_Follow};
int CPUMoveTick(int state){
    unsigned char ballHit = ~(ballRow) & ~(CPUpaddleRow);
    switch(state){
        case CPU_start:
        state = CPU_Wait;
        break;
        case CPU_Wait:
        if(ballPattern >0x01 && xDirection == 0x01 && (rand() % 90 == 0)){
            state = CPU_Follow;
        }
        break;
        case CPU_Follow:
            state = CPU_Wait;
        default:
        state = CPU_start;
        break;
    }
    switch(state){
        case CPU_start:
        break;
        case CPU_Follow:
            if(ballHit != 0 && yDirection == 1 && CPUpaddleRow != 0xFC){
                CPUpaddleRow = (CPUpaddleRow >> 1) | 0x80;
            }
            else if(ballHit != 0 && yDirection == 2 && CPUpaddleRow != 0xE7){
                CPUpaddleRow = (CPUpaddleRow << 1) | 0x01;
            }
        break;
    }

    return state;
}

enum createMatrixSM {create};
unsigned char pattern[5];
unsigned char row[5] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF};
int createMatrix(int state){
    switch(state){
        case create:
        break;
        default:
        state = create;
        break;
    }
    switch(state){
        case create:;
            unsigned char ballRowCopy = ~ballRow;
            unsigned char paddleRowCopy = ~paddleRow;
            unsigned char CPUpaddleRowCopy = ~CPUpaddleRow;
            for(unsigned int i = 0; i < 5 ; i++){
                pattern[i] = 0x00;
                if((ballRowCopy & 0x01) == 1){
                    pattern[i] = ballPattern;
                }
                if((paddleRowCopy & 0x01) == 1){
                    pattern[i] = pattern[i] | paddlePattern; 
                }
                if((CPUpaddleRowCopy & 0x01) == 1){
                    pattern[i] = pattern[i] | CPUpaddlePattern; 
                }
                ballRowCopy = ballRowCopy >> 1;
                paddleRowCopy = paddleRowCopy >> 1;
                CPUpaddleRowCopy = CPUpaddleRowCopy >> 1;
            }
        break;
        default:
        break;
    }
    return state;
}

enum Display_SM{display};
unsigned char j = 0;
int DisplayTick(int state){
    switch(state){
        case display:
        break;
        default:
        state = display;
        break;
    }
    switch(state){
        case display:
        PORTC = pattern[j];
        PORTD = row[j];
        j++;
        if(j > 4){
            j = 0;
        }
        break;
        default:
        break;
    }
    return state;
}


int main(void) {
    srand(time(NULL));
    /* Insert DDR and PORT initializations */
    DDRA = 0xF0; PORTA = 0x0F;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    static task task1, task2, task3, task4, task5;
    task *tasks[] = { &task1, &task2, &task3, &task4, &task5};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &MoveTick;

    task2.state = start;
    task2.period = ballSpeed;
    task2.elapsedTime = task2.period;
    task2.TickFct = &Demo_Tick;

    task3.state = start;
    task3.period = 1;
    task3.elapsedTime = task3.period;
    task3.TickFct = &CPUMoveTick;

    task4.state = start;
    task4.period = 1;
    task4.elapsedTime = task4.period;
    task4.TickFct = &createMatrix;

    task5.state = display;
    task5.period = 1;
    task5.elapsedTime = task5.period;
    task5.TickFct = &DisplayTick;

    TimerSet(1);
    TimerOn();
    unsigned short i;
    /* Insert your solution below */
    while (1) {
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                if(i == 1){
                    tasks[i]->period = ballSpeed;
                }
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
