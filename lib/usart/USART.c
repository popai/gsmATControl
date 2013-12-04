/******************************************************************************


 USART Core
 ----------
 This module is used for doing serial communication using USART.

 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"

void USARTInit(uint16_t ubrrvalue)
{
	//Setup q
	UQFront = UQEnd = -1;

	//Set Baud rate
	//UBRRH=(unsigned char)(ubrrvalue>>8);
	//UBRRL=(unsigned char)ubrrvalue;

	UBRR0H = (unsigned char) (ubrrvalue >> 8); //Setting baudrate
	UBRR0L = (unsigned char) ubrrvalue; //Setting baudrate

	/*Set Frame Format

	 Asynchronous mode
	 No Parity
	 1 StopBit
	 char size 8

	 */

	//UCSRC=(1<<URSEL)|(3<<UCSZ0);(1 << USBS0) |
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8N1...see Datasheet for more information

	/*Enable Interrupts
	 RXCIE- Receive complete
	 UDRIE- Data register empty

	 Enable The recevier and transmitter

	 */

	//UCSRB=(1<<RXCIE)|(1<<RXEN)|(1<<TXEN);( 1 << RXCIE0 )|
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //Enable receiver and transmitter
	sei();

}

//The USART ISR
ISR(USART_RXC_VECT)
{
	//Read the data
	char data = UDR0;

	//Now add it to q

	if (((UQEnd == RECEIVE_BUFF_SIZE - 1) && UQFront == 0)
			|| ((UQEnd + 1) == UQFront))
	{
		//Q Full
		UQFront++;
		if (UQFront == RECEIVE_BUFF_SIZE)
			UQFront = 0;
	}

	if (UQEnd == RECEIVE_BUFF_SIZE - 1)
		UQEnd = 0;
	else
		UQEnd++;

	URBuff[UQEnd] = data;

	if (UQFront == -1)
		UQFront = 0;

}

char UReadData()
{
	char data;

	//Check if q is empty
	if (UQFront == -1)
		return 0;

	data = URBuff[UQFront];

	if (UQFront == UQEnd)
	{
		//If single data is left
		//So empty q
		UQFront = UQEnd = -1;
	}
	else
	{
		UQFront++;

		if (UQFront == RECEIVE_BUFF_SIZE)
			UQFront = 0;
	}

	return data;
}

void UWriteData(char data)
{
	//Wait For Transmitter to become ready
	//while (!(UCSRA & (1 << UDRE)));
	while (!(UCSR0A & (1 << UDRE0)))
		;

	//Now write
	UDR0 = data;
}

uint8_t UDataAvailable()
{
	if (UQFront == -1)
		return 0;
	if (UQFront < UQEnd)
		return (UQEnd - UQFront + 1);
	else if (UQFront > UQEnd)
		return (RECEIVE_BUFF_SIZE - UQFront + UQEnd + 1);
	else
		return 1;
}

void UWriteString(const char *str)
{
	while ((*str) != '\0')
	{
		UWriteData(*str);
		str++;
	}
}

void UWriteInt(int i)
{

	//! Local variables
	int ii;
	unsigned char int_buf[5];

	if (i < 0) //Integer is negative
	{
		i = -i; //Convert to positive Integer
		UWriteData('-'); //Print - sign
	}

	for (ii = 0; ii < 5;) //Convert Integer to char array
	{
		int_buf[ii++] = '0' + i % 10; //Find carry using modulo operation
		i = i / 10; //Move towards MSB
	}
	do
	{
		ii--;
	} while ((int_buf[ii] == '0') && (ii != 0)); //Remove leading 0's
	do
	{
		UWriteData(int_buf[ii--]);
	} while (ii >= 0); //Print int->char array convertion

}

void UReadBuffer(void *buff, uint16_t len)
{
	uint16_t i;
	for (i = 0; i < len; i++)
	{
		((char*) buff)[i] = UReadData();
	}
}
void UFlushBuffer()
{
	while (UDataAvailable() > 0)
	{
		UReadData();
	}
}

/***********************************************************
 *					 SERIAL READ
 Function read data on serial line
 - @citit: char array to hold data
 **********************************************************/
int SerialRead(char *citit)
{
	int i = 0;
	while (UDataAvailable())
	{
		char c = UReadData();
		if ((c != '\r') && (c != '\n'))
		{
			citit[i++] = c;
		}
		else
			citit[i++] = '*';
	}
	if (i > 0)
		citit[i - 1] = '\0';
//return str;
	return i;
}
/*
 void SerialRead(char *citit)
 {
 while (UDataAvailable() > 0)
 {
 *citit++ = UReadData();
 }
 *citit++ = '\0';
 UFlushBuffer();
 }
 */
