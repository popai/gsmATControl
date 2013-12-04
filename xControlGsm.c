/*
 * main.c
 *
 *  Created on: Apr 14, 2013
 *      Author: popai
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>

#include "gsm.h"
#include "lib/usart/usart.h"
#include "lib/adc/adc.h"
#include "pinDef.h"
#include "cmd.h"

void setup();
void loop();

int main(void)
{
	setup();
	loop();
}

void setup()
{

// Add your initialization code here
	USARTInit(103);
	InitADC();
	pinSetUp();

	UWriteString("Program pornit");
	UWriteData(0x0D);
	UWriteData(0x0A);

	if (AT_INIT() != 1)
		PORTB = (1 << PB5);
}

void loop()
{
	int8_t config = 0, cfgpc = 0;
	uint16_t contor = 500;

	if ((PINC & (1 << PC3)) == 0)
	{
		config = 1;
	}

	if ((PINC & (1 << PC4)) == 0)
	{
		DellEprom();
		return;
	}

	if ((PINC & (1 << PC5)) == 0)
	{
		cfgpc = 1;
	}

	char read[128];
	char teln[11];
	int id = 0;

	while (1)
	{
		_delay_ms(3000);
		if (contor == 500)
		{
			contor = 0;
			BateryFull();
		}
		++contor;

		if (cfgpc)
		{
			SerialRead(read);
			if (strlen(read) != 0)
			{
				if (strstr_P(read, PSTR("citeste")) != 0)
				{
					for (int i = 0; i <= 512; i += 18)
						ReadEprom(read, i);
				}
				else
					CfgCmd(read);
				*read = 0x00;
			}
		}
		else if (config)
		{
			_delay_ms(100);
			id = WaitForMsg();
			if (id > 0)
			{
				ReadSms(id, teln, read);
				Config(teln, read);
				//_delay_ms(250);
				DellSms(id);
			}
			*read = 0x00;
			*teln = 0x00;
			id = 0;

		}
		else
		{
			VerificIN();
			//SerialRead(data);
			//delay(100);

			id = WaitForMsg();
			if (id > 0)
			{
				ReadSms(id, teln, read);
				//UWriteString(teln);
				//UWriteData(0x0D);
				//UWriteData(0x0A);
				//UWriteString(read);
				Comand(teln, read);
				//Comand(pfonnr, "test 1");
				//SendSms("012345678913", data);
				//_delay_ms(250);
				DellSms(id);
			}
			*read = 0x00;
			*teln = 0x00;
			id = 0;
			UWriteString("AT");
			UWriteData(0x0D);
			UWriteData(0x0A);

		}
	}
}

