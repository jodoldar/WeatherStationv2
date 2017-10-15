/**
 * Main file of the WeatherStation project
 * Author: Josep Dols
 * Version: 0.0.1
 * 
 * The purpose of this file is the next:
 * 		Retrieve the data from the weather station
 * 		Parse the information received
 * 		Send the information to the Weather Underground web page
 * 		Keep a copy of the information for future processing
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <usb.h>
 #include "WeatherStat.h"
 #include "te923aux.h"
 #include "te923driver.h"
 
  int main(int argc, char *argv[])
 {
	 printf("Variables declaration...");
	 char sUrlMessage[BUFFER_NET_LEN];
	 struct usb_dev_handle *stUsbDevice;
	 Te923DataSet_t *stDataSet;
	 int i;
	 printf(" OK\n");
	 
	 printf("Variables initialization...");
	 memcpy(sUrlMessage,"\0",strlen(sUrlMessage));
	 stUsbDevice = (usb_dev_handle*)malloc(sizeof(stUsbDevice));
	 memset(stUsbDevice,0,sizeof(stUsbDevice));
	 stDataSet = (Te923DataSet_t*)malloc(sizeof(Te923DataSet_t));
	 memset(stDataSet,0,sizeof(Te923DataSet_t));
	 i=0;
	 printf(" OK\n");
	 
	 // Retrieve information from the device
	 printf("Retrieving information from the device...");
	 stUsbDevice = te923_handle();
	 get_te923_lifedata(stUsbDevice,stDataSet);
	 te923_close(stUsbDevice);
	 printf(" OK\n");
	 
	 // Create the URL with the data received
	 printf("Creating an URL with information from the device...");
	 printf(" OK\n");
	 
	 // Envio de la informacion a Weather Underground
	 printf("Sending information to Weather Underground... ");
	 sprintf(sUrlMessage,"ID=ICOMUNID85&PASSWORD=1142aee1&action=updateraw&dateutc=now&humidity=80&baromin=2992&tempf=55");
	 
	 if(iSendDataToWUService(sUrlMessage))
	 {
		 perror("ERROR: Failure in the communication with the Weather Underground services to send the data.\n");
		 printf(" FAIL\n");
	 }else
	 {
		 printf(" OK\n"); //Data sent to Weather Underground\n");
	 }
	 return 0;
 } 
 
 void vCreateUrlFromData(Te923DataSet_t *stInputDataSet, char *sOutputUrl)
 {
	 printf("Information: \n");
	 printf("Timestamp: %u\n", stInputDataSet->timestamp);
	 for(i=0;i<6;i++){
		 printf("Temperature[%d]: %0.2f\n",i, stInputDataSet->t[i]);
	 }
	 for(i=0;i<6;i++){
		 printf("Humidity[%d]: %d\n",i, stInputDataSet->h[i]);
	 }
	 printf("Wind chill: %0.1f\n", stInputDataSet->wChill);
	 printf("Pressure: %0.1f\n", stInputDataSet->press);
	 
	 printData(stInputDataSet,":");
 }
 
 int iSendDataToWUService(char *sMessage)
 {
	 int iPortNumber;
	 char sWUHost[BUFFER_NET_LEN];
	 struct hostent *stServer;
	 struct sockaddr_in stServ_addr;
	 int iSocketFD;
	 int iBytesSent, iBytesReceived;
	 int iSent, iReceived;
	 int iTotal;
	 char sRequest[BUFFER_NET_LEN];
	 char sResponse[BUFFER_NET_LEN];
	 
	 iPortNumber = 80;
	 memcpy(sWUHost,"\0",strlen(sWUHost));
	 memcpy(sRequest,"\0",strlen(sRequest));
	 iSocketFD = 0;
	 iBytesSent = 0;
	 iBytesReceived = 0;
	 iSent = 0;
	 iReceived = 0;
	 iTotal = 0;
	 memcpy(sResponse,"\0",strlen(sResponse));
	 
	 sprintf(sWUHost,"weatherstation.wunderground.com");
	 sprintf(sRequest,"GET /weatherstation/updateweatherstation.php?%s HTTP/1.1\r\n\r\n",sMessage);
	 
	 if(DEBUG_NET){
		printf("Request:\n%s\n",sRequest);
		printf("Longitude: %d\n",strlen(sRequest));
	 }
	 
	 iSocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	 if(DEBUG_NET){
		printf("Socket created\n");
	}
	 if(iSocketFD < 0)
	 {
		perror("ERROR: Fail at opening the socket.\n");
		 return SEND_DATA_ERROR;
	 }
	 
	 stServer = gethostbyname(sWUHost);
	 if(DEBUG_NET){
		printf("Host addres resolved\n");
		printf("%s\n",stServer->h_name);
	}
	 if(stServer == NULL)
	 {
		perror("ERROR: The host doesn't exist.\n");
		 return SEND_DATA_ERROR;
	 }
	 
	 memset(&stServ_addr,0,sizeof(stServ_addr));
	 stServ_addr.sin_family = AF_INET;
	 stServ_addr.sin_port = htons(iPortNumber);
	 memcpy(&stServ_addr.sin_addr.s_addr, stServer->h_addr,stServer->h_length);
	 
	 if(connect(iSocketFD,(struct sockaddr *)&stServ_addr,sizeof(stServ_addr)) < 0)
	 {
		 perror("ERROR: The connection can't be established with the host.\n");
		 return SEND_DATA_ERROR;
	 }
	 if(DEBUG_NET){
		printf("Connection established with the host: %s\n", sWUHost);
	}
	 
	 iTotal = strlen(sRequest);
	 iSent = 0;
	 do
	 {
		 iBytesSent = write(iSocketFD,sRequest+iSent,iTotal-iSent);
		 if(DEBUG_NET){
			printf("%d\n",iBytesSent);
		}
		 if(iBytesSent < 0)
		 {
			 perror("ERROR: The message can't be written in the socket.\n");
			 return SEND_DATA_ERROR;
		 }
		 if(iBytesSent == 0)
		 {
			 break;
		 }
		 iSent += iBytesSent;
	 }while(iSent < iTotal);
	 if(DEBUG_NET){
		printf("Message sent.\n%s\n",sRequest);
	 }
	 
	 iTotal = BUFFER_NET_LEN; 	 
	 iReceived = 0;
	 
	 do
	 {
		 iBytesReceived = read(iSocketFD,sResponse+iReceived,iTotal-iReceived);
		 if(DEBUG_NET){
			printf("%d\n",iBytesReceived);
		}
		 if(iBytesReceived < 0)
		 {
			 perror("ERROR: The response can't be read from the socket.\n");
			 return SEND_DATA_ERROR;
		 }
		 if(iBytesReceived == 0)
		 {
			 break;
		 }
		 iReceived += iBytesReceived;
	 }while(iReceived < iTotal);
	 if(DEBUG_NET){
		printf("Response received.\n");
	}
	 
	 if(iReceived == iTotal)
	 {
		 perror("ERROR: The response is bigger than the buffer.\n");
	 }
	 
	 close(iSocketFD);
	 if(DEBUG_NET){
		printf("Connection closed.\n");
		printf("Response: %s\n", sResponse);
	}
	 return 0;
 }
 
