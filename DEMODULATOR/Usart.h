#include <inttypes.h>
#ifndef USART_H_
#define USART_H_
#define MAX_LEN 7
typedef struct {
	uint8_t HEAD;
	uint8_t TAIL;
	uint16_t UNHANDLED;
	uint8_t buff[MAX_LEN];
}Buffer;

void Buffer_Write(uint8_t data);
uint8_t Buffer_Read();
uint8_t Buffer_DataAvailable();

void USART_Init();
void USART_Write(uint8_t* data);
uint8_t USART_Read(void);

#endif 
