#include "Usart.h"
#include <avr/interrupt.h>

Buffer mass1;

void Buffer_Write(uint8_t data){
	if(mass1.HEAD==mass1.TAIL && mass1.UNHANDLED==MAX_LEN){
		mass1.buff[mass1.HEAD]=data;
		mass1.HEAD++;
		if(mass1.HEAD>MAX_LEN-1)mass1.HEAD=0;
		mass1.TAIL++;
		if(mass1.TAIL>MAX_LEN-1)mass1.TAIL=0;
	}else{
		mass1.buff[mass1.HEAD]=data;
		mass1.UNHANDLED++;	
		mass1.HEAD++;
		if(mass1.HEAD>MAX_LEN-1)mass1.HEAD=0;
	}
}
uint8_t Buffer_Read(){
	mass1.TAIL++;
	if(mass1.TAIL>MAX_LEN-1)mass1.TAIL=0;
	mass1.UNHANDLED--;
	if(mass1.TAIL==0){
		return mass1.buff[MAX_LEN-1];
	}
	else{
		return mass1.buff[mass1.TAIL-1];
	}
}
uint8_t Buffer_DataAvailable(){
	if(mass1.HEAD==mass1.TAIL){
		if(mass1.UNHANDLED==0){
			return 0;
		}else{ 
			return 1;
		}
	}else{
		return 1;
	}
}

ISR(USART_RX_vect){
	UCSR0B&=~(1<<RXCIE0);
	Buffer_Write(UDR0);
	UCSR0B|=1<<RXCIE0;
}

void USART_Init(){
	UCSR0B|=(1<<RXCIE0)|(1<<TXEN0)|(1<<RXEN0);
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ00);
	UBRR0L = 51;
}
void USART_Write(uint8_t* data){
	while (!(UCSR0A&(1<<UDRE0)));
	UDR0 = *data;
}
uint8_t USART_Read(void){
	while (!(UCSR0A&(1<<RXC0)));
	return UDR0;
}