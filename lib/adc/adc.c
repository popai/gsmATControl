/******************************************************************************

 ADC Core
 ----------
 This module is used for interfacing AVR's Inbuilt ADC(Analog To Digital Convertor).

 *******************************************************************************/

#include <avr/io.h>

#include "adc.h"

void InitADC()
{
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (7 << ADPS0);
}

uint16_t ReadADC(uint8_t ch)
{
	//Select ADC Channel ch must be 0-7
	ch = ch & 0b00000111;

	ADMUX &= 0b11100000;
	ADMUX |= ch;

	//Start Single conversion
	ADCSRA |= (1 << ADSC);

	//Wait for conversion to complete
	while (!(ADCSRA & (1 << ADIF)))
		;

	//Clear ADIF by writing one to it
	ADCSRA |= (1 << ADIF);

	return (ADC);
}
