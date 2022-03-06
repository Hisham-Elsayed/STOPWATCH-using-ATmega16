/*
 * Mini_Project2.c
 *
 * Created on: Sep 13, 2021
 * Author: Hisham Elsayed
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char SEC=0;
unsigned char MIN=0;
unsigned char HOUR=0;




ISR (TIMER1_COMPA_vect)
{
	SEC ++;
	if (SEC == 60)
	{
		SEC=0;
		MIN ++;          // start counting minutes and reset seconds //
	}
	if (MIN == 60)
	{
		SEC=0;
		MIN=0;
		HOUR ++;        // start counting hours and reset both minutes and seconds //
	}
	if (HOUR == 24)
	{
		SEC=0;
		MIN=0;
		HOUR=0;         // start count from the beginning //
	}

}

void TIMER1_init()
{
	TCNT1=0;
	TCCR1A |= (1<<FOC1A);    // non-PWM mode //
	TCCR1B |= (1<<CS10) | (1<<CS11) | (1<<WGM12);  //  pre-scaling mode to 64//
	TIMSK |= (1<<OCIE1A);  // Enable compare Interrupt for TIMER1 //
	OCR1A = 15625;          // set compare value to count every 1 second
}

void INT0_RESET()
{
	MCUCR |= (1<<ISC01);   // falling edge INTO interrupt request //
	GICR|= (1<<INT0);      // Enable external interrupt 0
	DDRD &= ~(1<<PD2);     // set PD2 as input pin //
	PORTD |= (1<<PD2);            // Enable internal pull up //

}

ISR (INT0_vect)
{
	// To RESET Stop watch //
	SEC=0;
	MIN=0;
	HOUR=0;
}

void INT1_PAUSE()
{
	DDRD &= ~(1<<PD3);   // set PD3 as input pin //
	MCUCR |= (1<<ISC11) | (1<<ISC10);  // enable rising edge of INT1 //
	GICR |= (1<<INT1);   // Enable external interrupt 1 //

}

ISR (INT1_vect)
{
	//if(PIND & (1<<PD3)) // check if the first push button at PA0 is pressed or not
	//{
		//_delay_ms(30);
		//if (PIND & (1<<PD3))  // second check due to switch bouncing //
		//{
			TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);  //  No clock source (Timer/Counter stopped). TIMER1 (PAUSE) //
		//}
		//while(PIND & (1<<PD3)){} // wait until the switch is released //
	//}
}

ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS11);  // Enable Timer/Counter for TIMER1 (RESUME) //
}

void INT2_RESUME()
{
	DDRB &= ~(1<<PB2); // set PB2 as input pin //
	PORTB |= (1<<PB2);  // Enable internal pull up //
	MCUCSR &= ~(1<<ISC2); // enable falling edge of INT2 //
	GICR |= (1<<INT2);  // Enable external interrupt 2 //
}


int main(void)
{
	DDRC |= 0x0F;  // set all four pins of PORTC as output pins //
	DDRA |=0x3F;  // set first 6 pins of PORTA as output pins //
	PORTC = 0x00;
	PORTA = 0x3F;
	SREG |= (1<<7); // Enable I-bit //
	TIMER1_init();
	INT0_RESET();
	INT1_PAUSE();
	INT2_RESUME();
	while (1)
	{
		PORTA = (1<<0);    // enable PA0 (SEC1)
		PORTC = SEC % 10;   // increment SEC from 0 to 9
		_delay_ms(2);
		PORTA = (1<<1);     // enable PA1 (SEC2)
		PORTC = SEC / 10;   // increment only when number is divisible by 10
		_delay_ms(2);
		PORTA = (1<<2);      // enable PA2 (MIN1)
		PORTC = MIN % 10;
		_delay_ms(2);
		PORTA = (1<<3);       // enable PA3 (MIN2)
		PORTC = MIN / 10;
		_delay_ms(2);
		PORTA = (1<<4);        // enable PA4 (HOUR1)
		PORTC = HOUR % 10;
		_delay_ms(2);
		PORTA = (1<<5);          // enable PA5 (HOUR2)
		PORTC = HOUR / 10;
		_delay_ms(2);
	}
}

