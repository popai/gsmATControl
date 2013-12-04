/*
 * sim300.c
 *
 *  Created on: Apr 14, 2013
 *      Author: popai
 */

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "gsm.h"
#include "lib/usart/usart.h"

//#define DEBUG 1

//A common buffer used to read response from pfon
//char buffer[128];

const prog_char ATE0[] PROGMEM = "ATE0"; //< Echo off
const prog_char AT_CNMI[] PROGMEM = "AT+CNMI=1,1,0,0,1"; //< Identification of new sms
const prog_char AT_CPMS[] PROGMEM = "AT+CPMS=\"ME\""; //< Preferred storage
const prog_char AT_CMGF[] PROGMEM = "AT+CMGF=1"; //< Send SMS string format 0 = PDU, 1 = text mod
const prog_char AT_CBC[] PROGMEM = "AT+CIND?"; //< Batery state
const prog_char CMTI[] PROGMEM = "CMTI";
const prog_char CBC[] PROGMEM = "CIND";
const prog_char OK[] PROGMEM = "OK";
//const prog_char ERROR[] PROGMEM = "ERROR";
const prog_char CMD_ERR[] PROGMEM = "Comanda ne scrisa";

int8_t AT_INIT()
{
	int retVal = 1;
	char buffer[24];
	//write mesage in EEPPROM
	eeprom_read_block(buffer, (int*) 486, 18);
	if (strlen(buffer) == 0)
	{
		strcpy_P(buffer, CMD_ERR);
		eeprom_write_block(buffer, (int*) 486, 18);
		strcpy_P(buffer, 0x00);
	}

	PORTB = (1 << PB4);
	_delay_ms(1000);

	strcpy_P(buffer, ATE0);
	UWriteString(buffer);
	UWriteData(0x0A);
	UWriteData(0x0D);

	_delay_ms(1000);

	SerialRead(buffer);
	if (strstr_P(buffer, OK) == 0)
		retVal = -1;
#ifdef DEBUG
	UWriteString(buffer);
	UWriteInt(retVal);
#endif

	//PORTB ^= (1 << PB4);
	_delay_ms(1000);

	strcpy_P(buffer, AT_CNMI);
	UWriteString(buffer);
	UWriteData(0x0A);
	UWriteData(0x0D);

	//PORTB ^= (1 << PB4);
	_delay_ms(1500);

	SerialRead(buffer);
	if (strstr_P(buffer, OK) == 0)
		retVal = -2;
#ifdef DEBUG
	UWriteString(buffer);
	UWriteInt(retVal);
#endif

	//PORTB ^= (1 << PB4);
	_delay_ms(1000);

	strcpy_P(buffer, AT_CPMS);
	UWriteString(buffer);
	UWriteData(0x0A);
	UWriteData(0x0D);

	//PORTB ^= (1 << PB4);
	_delay_ms(1000);
	//PORTB ^= (1 << PB4);

	SerialRead(buffer);
	if (strstr_P(buffer, OK) == 0)
		retVal = -3;
#ifdef DEBUG
	UWriteString(buffer);
	UWriteInt(retVal);
#endif

	_delay_ms(1000);
	//PORTB ^= (1 << PB4);

	strcpy_P(buffer, AT_CMGF);
	UWriteString(buffer);
	UWriteData(0x0A);
	UWriteData(0x0D);

	_delay_ms(1000);
	//PORTB &= ~(1 << PB4);

	SerialRead(buffer);
	if (strstr_P(buffer, OK) == 0)
		retVal = -4;
#ifdef DEBUG
	UWriteString(buffer);
	UWriteInt(retVal);
#endif
	PORTB &= ~(1 << PB4);
	return retVal;
}

int8_t DellSms(int id)
{
	PORTB = (1 << PB4);
	UFlushBuffer();
	_delay_ms(500);
	//String for storing the command to be sent
	char cmd[16];

	//Build command string
	sprintf_P(cmd, PSTR("AT+CMGD=%d"), id);

	//Send Command
	UWriteString(cmd);
	UWriteData(0x0D); // CR
	UWriteData(0x0A); // LF

	//uint8_t len = WaitForResponse(1000);
	_delay_ms(1000);
	SerialRead(cmd);

	//Check if the response is OK
	if (strstr_P(cmd, OK) != 0)
	{
		PORTB &= ~(1 << PB4);
		return 1;
	}
	else
	{
		PORTB &= ~(1 << PB4);
		PORTB = (1 << PB5);
		return 0;
	}
}

int WaitForMsg()
{
	char buffer[24];
	char *start;
	buffer[0] = 0x00;
	SerialRead(buffer);
	if (strstr_P(buffer, CMTI) != 0)
	{
		start = strchr(buffer, ',');
		start++;
		//int8_t i;
		//for(int8_t i = 0; start[i] != '*'; ++i)
		int p = atoi(start);
		if (p > 0)
			return p;
	}
	return 0;
}

int8_t ReadSms(int id, char* nrtel, char *msg)
{
	//Clear pending data in queue
	PORTB = (1 << PB4);

	//String for storing the command to be sent
	char cmd[16];
	char buffer[128];
	//Build command string
	sprintf_P(cmd, PSTR("AT+CMGR=%d"), id);
	_delay_ms(500);
	//Send Command
	UWriteString(cmd);
	UWriteData(0x0A);
	UWriteData(0x0D);

	_delay_ms(3000);
	SerialRead(buffer);
	_delay_ms(50);
	if (strstr_P(buffer, OK) != 0) //Read = OK
	{
		char *start;
		//ghet pfone number
		start = strchr(buffer, ',');
		start += 4;
		strncpy(nrtel, start, 10);
		nrtel[10] = 0x00;

		//ghet mesage
		start = strchr(start, '*');
		//start++;
		while (*start == '*')
			start++;
		int8_t i;
		for (i = 0; start[i] != '*'; ++i)
			msg[i] = start[i];
		msg[i] = 0x00;
		//UWriteString(msg);
		//UWriteData(0x0A);
		//UWriteData(0x0D);
		//strcpy(buffer, "\0");
		PORTB &= ~(1 << PB4);
		return 1;
	}
	PORTB &= ~(1 << PB4);
	PORTB = (1 << PB5);
	return 0;
}

void SendSms(const char *num, const char *msg)
{
	PORTB = (1 << PB4);
	_delay_ms(1000);

	char cmd[25];

	sprintf_P(cmd, PSTR("AT+CMGS=\"%s\""), num);

	//Send Command
	UWriteString(cmd);
	UWriteData(0x0D); // CR
	UWriteData(0x0A); // LF

	_delay_ms(2000);
	UWriteString(msg);
	UWriteData(0x1A); // Ctrl+Z
	UWriteData(0x0D); // CR
	UWriteData(0x0A); // LF

	_delay_ms(1000);
	PORTB &= ~(1 << PB4);

}

int8_t BateryFull()
{
	char *start;
	int bat = 0;
	char buffer[24];

	strcpy_P(buffer, AT_CBC);
	UWriteString(buffer);
	UWriteData(0x0D); // CR
	UWriteData(0x0A); // LF

	_delay_ms(500);

	SerialRead(buffer);
	if (strstr_P(buffer, CBC) != 0)
	{
		start = strchr(buffer, ':');
		start++;
		//start[strlen(start)-4] = '\0';
		//UWriteString(start);
		bat = atoi(start);
		//UWriteString(bat);
	}
	if (bat == 5)
	{
		PORTD |= (1 << PD7);
		return 1;
	}

	else
	{
		PORTD &= ~(1 << PD7);
		return 0;
	}
}

