/*
 * GccApplication4.c
 *
 * Created: 11/2/2022 12:40:12 PM
 * Author : islam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 1000000
#include <util/delay.h>
#include <stdlib.h>
void SETBIT(volatile uint8_t *BASE , int pin);
void CLEARBIT(volatile uint8_t *BASE , int pin);
void TOGGLEBIT(volatile uint8_t *BASE , int pin);
void avoid_obstacle();

static volatile int pulse = 0; //A variable to store the width of the pulse (time)
static volatile int flag = 0; //A flag to tell you when to start/stop counting 
volatile float distance = 0; //A variable in which we will store the distance 
char out_flag=0;             //A flag to make sure we don't measure distance again after avoiding the obstacle

int main(void)
{
    /* Replace with your application code */
	SETBIT(&DDRD,PD1);    //Set pin 1 in PORTD to o/p as it is the trigger
	CLEARBIT(&DDRD,PD2);  //Set pin 2(INT0 which is external interrupt 0) in PORTD to i/p as it is the echo 
	SETBIT(&GICR,INT0);   //Enable interrupt zero
	SETBIT(&MCUCR,ISC00); //Interrupt at any logical change to start count when 0 > 1 and stop count when 1>0
	sei();				//A function in the library avr/interrupt.h to enable the global interrupts
	CLEARBIT(&PORTD,PD1); //make sure that trigger pin is LOW before starting 
	_delay_us(2);
    while (1) 
    {
		/*
		
		The motion control code
		
		*/
		if(flag==0 && out_flag==0) //if the counter is not counting then send and receive a signal to know the distance 
		{
			distance = pulse *( (1/1000000) * 0.5 * 340); //the distance is number of clk cycles*clk period(1/Fclk)*0.5*sound speed
			if(distance<0/*Will be evaluated*/)
			{
				out_flag=1; //Don't measure distance again
				avoid_obstacle();
			}
			SETBIT(&PORTD,PD1);
			_delay_us(10);
			CLEARBIT(&PORTD,PD1);
		}

    }
}

void SETBIT(volatile uint8_t *BASE , int pin)
{
	*(BASE) |= (1<< pin);
}
void CLEARBIT(volatile uint8_t *BASE , int pin)
{
	*(BASE) &= ~(1<< pin);
}
void TOGGLEBIT(volatile uint8_t *BASE , int pin)
{
	*(BASE) ^= (1<< pin);
}
ISR(INT0_vect)
{
	switch(flag)
	{
		case 0	:
		SETBIT(&TCCR1B,CS10); //Start counting
		flag=1; //Change the flag to stop counting on the next interrupt
		break;
		case 1 :
		TCCR1B = 0 ;   //Stop the counter
		pulse = TCNT1; //TCNT is a register that start counting clock cycles when CS10 is 1
		TCNT1 = 0;     //To start counting from zero again next time
		flag=0;       //Reset the flag again so that we start counting the during the next interrupt
		break;
	}
}
void avoid_obstacle()
{
	//will be written
}