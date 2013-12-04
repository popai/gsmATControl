/*
 * cmd.c
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 */
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "cmd.h"
#include "pinDef.h"
#include "gsm.h"
#include "lib/usart/usart.h"
#include "lib/adc/adc.h"

//"Flash store comands etc are strings to store
const prog_char IN1[] PROGMEM = "IN1"; //adresa 18*2
const prog_char IN2[] PROGMEM = "IN2"; //adresa 18*3
const prog_char IN3[] PROGMEM = "IN3"; //adresa 18*4
const prog_char IN4[] PROGMEM = "IN4"; //adresa 18*5
const prog_char OUT1H[] PROGMEM = "OUT1L"; //adresa 18*6
const prog_char OUT1L[] PROGMEM = "OUT1H"; //adresa 18*7
const prog_char OUT2H[] PROGMEM = "OUT2L"; //adresa 18*8
const prog_char OUT2L[] PROGMEM = "OUT2H"; //adresa 18*9
const prog_char OUT3H[] PROGMEM = "OUT3L"; //adresa 18*10
const prog_char OUT3L[] PROGMEM = "OUT3H"; //adresa 18*11
const prog_char OUT4H[] PROGMEM = "OUT4L"; //adresa 18*12
const prog_char OUT4L[] PROGMEM = "OUT4H"; //adresa 18*13
const prog_char OUT5H[] PROGMEM = "OUT5L"; //adresa 18*14
const prog_char OUT5L[] PROGMEM = "OUT5H"; //adresa 18*15
const prog_char TMP1[] PROGMEM = "TMP1"; //adresa 18*16
const prog_char TMP2[] PROGMEM = "TMP2"; //adresa 18*17
const prog_char TMP3[] PROGMEM = "TMP3"; //adresa 18*18
const prog_char LOGIN[] PROGMEM = "LOGIN"; //adresa 18*19
const prog_char STARE_OUT[]PROGMEM = "STARE OUT"; //adresa 18*20
const prog_char STARE_TMP[] PROGMEM = "STARE TMP"; //adresa 18*21
const prog_char STARE_ALL[]PROGMEM = "STARE ALL"; //adresa 18*22

// The table to refer to my strings.
PROGMEM const char *comenzi[] =
{ IN1, IN2, IN3, IN4, OUT1H, OUT1L, OUT2H, OUT2L, OUT3H, OUT3L, OUT4H, OUT4L,
		OUT5H, OUT5L, TMP1, TMP2, TMP3, LOGIN };

int8_t in1 = 1, in2 = 1, in3 = 1, in4 = 1;

//write data on eeprom
int CfgCmd(char *inbuffer)
{
	int adresa = 18;
	//char *scriu;
	char comanda[7];

	for (int8_t i = 0; i < 18; ++i)
	{
		strcpy_P(comanda, (char*) pgm_read_word(&(comenzi[i]))); // Necessary casts and dereferencing, just copy.
		if (strstr(inbuffer, comanda) != 0)
		{

			inbuffer += strlen(comanda) + 1;
			adresa = 18 * (2 + i);
//#if DEBUG
			//Serial.print("Scriu la adresa ");
			UWriteInt(adresa);
			//Serial.print(": ");
			UWriteString(inbuffer);
			UWriteData(0x0A);
			UWriteData(0x0D);
//#endif
			eeprom_write_block(inbuffer, (int*) adresa, 18);
			return 1;
		}
	}
	return 0;
}

//read data from eeprom on specified adres
void ReadEprom(char* str_citit, int const adresa)
{
	//char str_citit[18];
	eeprom_read_block(str_citit, (int*) adresa, 18);
//#if DEBUG
	//Serial.print("citesc la adresa: ");
	UWriteInt(adresa);
	UWriteString(str_citit);
	UWriteData(0x0A);
	UWriteData(0x0D);
//#endif
	//return str_citit;
}

void DellEprom()
{
	for (int i = 0; i < 512; i++)
		eeprom_write_byte((uint8_t*) i, 0);
}
/*
 void DellPass()
 {
 for (int i = 18 * 19; i < 18 * 20; i++)
 EEPROM.update(i, 0);
 }
 */
void Comand(char *nrtel, char *inmsg)
{
	char buffer[24];
	char OK[3] = "OK";
	//bool cmdok = false;
	ReadEprom(buffer, 18);
	if (strcmp(nrtel, buffer) == 0)
	{

		ReadEprom(buffer, 18 * 6);
		//Serial.println(strcasecmp(buffer, inmsg));
		//if (strstr(inmsg,buffer) != 0)
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD1, LOW);
			PORTD &= ~(1 << PD2);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 7);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD1, HIGH);
			PORTD = (1 << PD2);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 8);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD2, LOW);
			PORTD &= ~(1 << PD3);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 9);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD2, HIGH);
			PORTD = (1 << PD3);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 10);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD3, LOW);
			PORTD &= ~(1 << PD4);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 11);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD3, HIGH);
			PORTD = (1 << PD4);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 12);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD4, LOW);
			PORTD &= ~(1 << PD5);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 13);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD4, HIGH);
			PORTD = (1 << PD5);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 14);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD5, LOW);
			PORTD &= ~(1 << PD6);
			SendSms(nrtel, OK);
			return;
		}

		ReadEprom(buffer, 18 * 15);
		if (strcasecmp(buffer, inmsg) == 0)
		{
			//digitalWrite(outD5, HIGH);
			PORTD = (1 << PD6);
			SendSms(nrtel, OK);
			return;
		}

		//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[18])));
		//if (strcasecmp(buffer, inmsg) == 0)
		if (strcasecmp_P(inmsg, STARE_OUT) == 0)
		{
			StareOUT(nrtel);
			return;
		}
		//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[19])));
		//if (strcasecmp(buffer, inmsg) == 0)
		if (strcasecmp_P(inmsg, STARE_TMP) == 0)
		{
			StareTMP(nrtel);
			return;
		}

		//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[20])));
		//if (strcasecmp(buffer, inmsg) == 0)
		if (strcasecmp_P(inmsg, STARE_ALL) == 0)
		{
			StareOUT(nrtel);
			StareTMP(nrtel);
			return;
		}

		ReadEprom(buffer, 486);
		SendSms(nrtel, buffer);
		return;
	}
	else
	{
		ReadEprom(buffer, 18 * 19);
		if (strcmp(buffer, inmsg) == 0)
		{
			eeprom_write_block(nrtel, (int*) 18, 18);
			strcpy_P(buffer, LOGIN);
			SendSms(nrtel, buffer);
		}
	}
}

//write the sms string for commands
void Config(char *nrtel, char *inmsg)
{
	char buffer[18];
	int adr = 18;
	if ((strlen(nrtel) != 0) && (strlen(inmsg) != 0))
	{

		//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[17])));
		if (strstr_P(inmsg, LOGIN) != 0)
		{
			eeprom_write_block(nrtel, (int*) adr, 18);
			CfgCmd(inmsg);

		}
		else
		{
			ReadEprom(buffer, 18);
			if (strcmp(nrtel, buffer) == 0)
				CfgCmd(inmsg);
		}
	}
}


void StareOUT(char *nrtel)
{
	char mesage[120];
	char buffer[18];
	//int i = 108;
	*mesage = 0x00;
	//if (digitalRead(outD1) == LOW)
	if ((PIND & (1 << PD2)) == 0)
	{
		ReadEprom(buffer, 108);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}
	else
	{
		ReadEprom(buffer, 126);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}

	//if (digitalRead(outD2) == LOW)
	if ((PIND & (1 << PD3)) == 0)
	{
		ReadEprom(buffer, 144);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}
	else
	{
		ReadEprom(buffer, 162);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}

	//if (digitalRead(outD3) == LOW)
	if ((PIND & (1 << PD4)) == 0)
	{
		ReadEprom(buffer, 180);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}
	else
	{
		ReadEprom(buffer, 198);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}

	//if (digitalRead(outD4) == LOW)
	if ((PIND & (1 << PD5)) == 0)
	{
		ReadEprom(buffer, 216);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}
	else
	{
		ReadEprom(buffer, 234);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}

	//if (digitalRead(outD5) == LOW)
	if ((PIND & (1 << PD6)) == 0)
	{
		ReadEprom(buffer, 252);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat(mesage, "\n\r");
		}

		//i += 18;
	}
	else
	{
		ReadEprom(buffer, 270);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			//strcat(mesage, "\n");
		}

		//i += 18;
	}
	if (strlen(mesage) != 0)
		SendSms(nrtel, mesage);
}

//calculate the temperature in grads Celsius dead by a 10K termistor
float Thermistor(int Tpin)
{
	float Vo;
	float logRt, Rt, T;
	float R = 9870; // fixed resistance, measured with multimeter
	// c1, c2, c3 are calibration coefficients for a particular thermistor
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
	//Vo = analogRead(Tpin);
	Vo = ReadADC(Tpin);
	Rt = R * (1024.0 / Vo - 1.0);
	logRt = log(Rt);
	T = (1.0 / (c1 + c2 * logRt + c3 * logRt * logRt * logRt)) - 273.15;
	return T;
}

void StareTMP(char *nrtel)
{
	char mesage[120];
	char buffer[18];
	char tmpe[32];
	int tmp, tmp1, tmp2;

	tmp = Thermistor(PC0);
	_delay_ms(10);
	tmp1 = Thermistor(PC0);
	_delay_ms(10);
	tmp2 = Thermistor(PC0);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 16);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	tmp = Thermistor(PC1);
	_delay_ms(10);
	tmp1 = Thermistor(PC1);
	_delay_ms(10);
	tmp2 = Thermistor(PC1);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 17);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	tmp = Thermistor(PC2);
	_delay_ms(10);
	tmp1 = Thermistor(PC2);
	_delay_ms(10);
	tmp2 = Thermistor(PC2);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 18);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %c", buffer, tmp, 'C');
		strcat(mesage, tmpe);
	}
	if (strlen(mesage) != 0)
		SendSms(nrtel, mesage);

}

void VerificIN()
{
	//char mesage[80];
	char nrtel[18];
	char buffer[18];

	if ((PINB & (1 << PB0)) == 0)
	{
		if (in1)
		{
			ReadEprom(nrtel, 18);
			in1 = 0;
			ReadEprom(buffer, 18 * 2);
			if (strlen(buffer) != 0)
				SendSms(nrtel, buffer);
		}
	}
	else
		in1 = 1;

	//if (digitalRead(inD2) == LOW && in2)
	if ((PINB & (1 << PB1)) == 0)
	{
		if (in2)
		{
			ReadEprom(nrtel, 18);
			in2 = 0;
			ReadEprom(buffer, 18 * 3);
			if (strlen(buffer) != 0)
				SendSms(nrtel, buffer);
		}
	}
	else
		in2 = 1;

	//if (digitalRead(inD3) == LOW && in3)
	if ((PINB & (1 << PB2)) == 0)
	{
		if (in3)
		{
			ReadEprom(nrtel, 18);
			in3 = 0;
			ReadEprom(buffer, 18 * 4);
			if (strlen(buffer) != 0)
				SendSms(nrtel, buffer);
		}
	}
	else
		in3 = 1;

	//if (digitalRead(inD4) == LOW && in4)
	if ((PINB & (1 << PB3)) == 0)
	{
		if (in4)
		{
			ReadEprom(nrtel, 18);
			in4 = 0;
			ReadEprom(buffer, 18 * 5);
			if (strlen(buffer) != 0)
				SendSms(nrtel, buffer);
		}
	}
	else
		in4 = 1;

}

