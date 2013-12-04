/*
 * cmd.h
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 */

#ifndef CMD_H_
#define CMD_H_


int CfgCmd(char *buffer);
void ReadEprom(char* str_citit, int const adresa);
void DellEprom();
//void DellPass();
void Config(char *nrtel, char *inmsg);
void Comand(char *nrtel, char *inmsg);
float Thermistor(int RawADC);
//void StareIN(char *nrtel);
void StareOUT(char *nrtel);
void StareTMP(char *nrtel);
void VerificIN();
//void StareALL(char *nrtel);

#endif /* CMD_H_ */
