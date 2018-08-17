#include <avr/io.h>
//LCD library
#include "io.c"	
#define SCALE 20
#define STOP 182
#define CLOCKWISE (STOP + SCALE)
#define COUNTERCLOCKWISE (STOP - SCALE)
#include <avr/interrupt.h>
#include "usart_ATmega1284.h"
#include "Timer.h"
typedef struct task {
	int state;						// Task’s current state
	unsigned long period;			// Task period
	unsigned long elapsedTime;		// Time elapsed since last task tick
	int (*TickFct)(int);			// Task tick function
} task;

#define tasksSize 0
task tasks[tasksSize];

void addTasks(){
	unsigned char i = 0;
	//tasks[i].state = 
	//tasks[i].period = 
	//tasks[i].elapsedTime =
	//tasks[i].TickFct = &
	//i++;

	tasks[i].state = 0;
	tasks[i].period = 0;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &ServoTick;
	i++;	
	
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

double freq[] = {82.41,110.00,146.83,196.00, 246.94};
/*
1 (E)	329.63 Hz	E4
2 (B)	246.94 Hz	B3
3 (G)	196.00 Hz	G3
4 (D)	146.83 Hz	D3
5 (A)	110.00 Hz	A2
6 (E)	82.41 Hz	E2	
*/

// function to initialize UART

static unsigned char servoChoice = 0x00;

void initServoMotor(){

	TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
	TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
	ICR1=4999;  //fPWM=50Hz

	OCR1A = STOP;

}

enum TickStates{Start, GetSignal, ServoChange} TickState;
void ServoTick(){
	
	switch(TickState){
		case Start: TickState = GetSignal; break;
		case GetSignal:
				if(USART_HasReceived(0)){
					servoChoice = 0x01;
					TickState = ServoChange;
					USART_Flush(0);
					
					
				}
				else if(USART_HasReceived(1)){
					servoChoice = 0x02;
					TickState = ServoChange;
					USART_Flush(1);
				}
			break;
		case ServoChange:
			//counterclockwise
			if(USART_HasReceived(0)){
					TickState = GetSignal;
					USART_Flush(0);
			}
			
				
			if(USART_HasReceived(1)){
				TickState = GetSignal;
				USART_Flush(1);
			}
			
			break;
	}
	
	switch(TickState){
		case Start:break;
		case GetSignal:break;
		case ServoChange:
			if(servoChoice == 1){
				OCR1A = CLOCKWISE
			}
			else if(servoChoice == 2){
				OCR1A = COUNTERCLOCKWISE
			}
			break;
	}
	
	
	PORTB = servoChoice;
}
int main(void)
{
	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TickState = 0;
	initUSART(0);
	initUSART(1);
	initServoMotor();
	USART_Flush(0);
	USART_Flush(1);
	addTasks();
	/*
	COUNTERCLOCKWISE -> works
	STOP -> COUNTERCLOCKWISE - 500
	*/

	//USART Protocol must wait for a receive
	while(1){
		ServoTick();
 	}
	return 0;	
}
	