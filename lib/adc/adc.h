#ifndef _ADC_H
#define _ADC_H

//Initialises the ADC Module
void InitADC();

//Start a ADC Conversion on channel ch (0-5)
//Returns the Converted value
uint16_t ReadADC(uint8_t ch);

#endif
