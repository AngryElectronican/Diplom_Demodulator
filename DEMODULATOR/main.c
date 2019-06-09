/*
 * ATMEGA_328_TIMER.c
 *
 * Created: 28.07.2018 23:54:26
 * Author : ANGRY_ELECTRONICAN
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>


#include "Timer.h"
#include "Usart.h"
#define MY_ADDR 2
#define ALL_ADDR 255

#define LED		PD7
#define PIN_OUT PD3
#define ZERO	PD2

#define TX		PD1
#define RX		PD0

Time time1=0;
uint8_t flag=0;
uint8_t state=0;
uint8_t counter=0;
uint8_t flag1=0;
char buf[8];
void ADC_Init();
void ADC_SetChannel(uint8_t channel);
void INT0_Init();

ISR(ADC_vect){
	static uint16_t max=0;
	static uint16_t min=0;
	static uint16_t delta=200;
	static uint16_t data=0;
	static uint16_t	read=0;
	static uint8_t i=0;
	uint8_t bit=0;
	data=ADC;
// 	uint8_t mass_debug[5]={0};
// 	for(int j=0;j<sprintf(mass_debug,"%d\n",data);j++){
// 		USART_Write(mass_debug+j);
// 	}
	if(data<=min){
		min=data;
	}
	if(data>max){
		max=data;
	}
	if (data>=((max+min)/2)){
		bit=0;
		max=data;
		min=max-delta;
		PORTD|=1<<LED;
	}else{
		min=data;
		max=min+delta;
		bit=1;
		PORTD&=~(1<<LED);
	}
	switch(state){
		case 0:
				if(bit==1){
					state=1;
					read=0;
				}
				break;
		case 1:
				read|=bit<<i;
				i++;
				if(i>=16){
					i=0;
					state=2;
				}
				break;
		case 2:
				if(bit!=read%2){
					//read=0;
					if ((0xFF&(read>>8)==MY_ADDR) || (0xFF&(read>>8)==ALL_ADDR)){
						OCR2B=(uint8_t)read&0xFF;
						}
				}
				state=0;
				break;
		}
	flag=0;
}

ISR(INT0_vect){
	Timer0_StartTimer(&time1);
	EIMSK&=~(1<<INT0);
	flag=1;
}

int main(void)
{
	DDRD|=(1<<TX)|(1<<LED)|(1<<PIN_OUT);
	PORTD|=(1<<RX)|(1<<PIN_OUT)|(1<<ZERO);
	USART_Init();
	Timer0_Init();
	INT0_Init();
	ADC_Init();
	ADC_SetChannel(0);
	TCCR2A|=(1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
	TCCR2B|=(1<<CS22);
	OCR2B=0; 
	sei();
    while (1) 
    {
		cli();
		if(Timer0_TimeIsOut(&time1,50)&&flag){
			flag=0;
			sei();
			if((EIFR>>INTF0)&1){
				EIFR|=1<<INTF0;
			}
			ADCSRA|=1<<ADSC;
			EIMSK|=1<<INT0;
		}
		sei();
    }
}

void ADC_Init(){
	ADMUX|=(1<<REFS0);
	ADCSRA|=(1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(0<<ADPS0)|(0<<ADPS1)|(0<<ADPS2);
}
void ADC_SetChannel(uint8_t channel){
	ADMUX=(ADMUX&0xF0)+channel;
}
void INT0_Init(){
	EICRA|=(1<<ISC01)|(1<<ISC00);
	EIMSK|=1<<INT0;
}