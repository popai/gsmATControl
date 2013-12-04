/*
 * gsm.h
 *
 *  Created on: Apr 14, 2013
 *      Author: popai
 */

#ifndef GSM_H_
#define GSM_H_

#include <inttypes.h>

//Public Interface
int8_t	AT_INIT();
int8_t DellSms(int id);
int WaitForMsg();
int8_t ReadSms( int id, char* nrtel, char *msg);
void SendSms(const char *num, const char *msg);
int8_t BateryFull();
int SerialRead(char *citit);

#endif /* GSM_H_ */
