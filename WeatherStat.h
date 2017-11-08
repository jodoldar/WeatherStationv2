#ifndef WEATHERSTAT_H_
#define WEATHERSTAT_H_

#include "te923aux.h"
#include "te923driver.h"
 
#define TRUE 1
#define FALSE 0

#define BUFFER_NET_LEN 1024

#define SEND_DATA_ERROR 1

#define DEBUG_NET 0
#define DEBUG_PARSE 0

void vCreateUrlFromData(Te923DataSet_t *stInputDataSet, char *sOutputUrl);
int iSendDataToWUService(char *sMessage);
int iSendDataToLocalServer(char *sMessage);

#endif /* WEATHERSTAT_H_*/
