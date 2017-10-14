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
 #include "WeatherStat.h"
 
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
	 char sResponse[4096*500];
	 fd_set desLectura;
	 
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
	 FD_ZERO(&desLectura);
	 
	 sprintf(sWUHost,"weatherstation.wunderground.com");
	 //sprintf(sRequest,"GET https://www.%s/weatherstation/updateweatherstation.php?%s HTTP/1.0\r\nHost: %s\r\n\r\n",sWUHost,sMessage,sWUHost);
	 sprintf(sRequest,"GET https://www.%s/weatherstation/updateweatherstation.php HTTP/1.0\r\nHost: %s\r\n\r\n",sWUHost,sWUHost);

	 printf("Request:\n%s\n",sRequest);
	 
	 iSocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	 FD_SET(iSocketFD,&desLectura);
	 if(iSocketFD < 0)
	 {
		 printf("ERROR: Fail at opening the socket.\n");
		 return SEND_DATA_ERROR;
	 }
	 
	 stServer = gethostbyname(sWUHost);
	 printf("%s\n",stServer->h_name);
	 if(stServer == NULL)
	 {
		 printf("ERROR: The host doesn't exist.\n");
		 return SEND_DATA_ERROR;
	 }
	 
	 memset(&stServ_addr,0,sizeof(stServ_addr));
	 stServ_addr.sin_family = AF_INET;
	 stServ_addr.sin_port = htons(iPortNumber);
	 memcpy(&stServ_addr.sin_addr.s_addr, stServer->h_addr,stServer->h_length);
	 
	 if(connect(iSocketFD,(struct sockaddr *)&stServ_addr,sizeof(stServ_addr)) < 0)
	 {
		 printf("ERROR: The connection can't be established with the host.\n");
		 return SEND_DATA_ERROR;
	 }
	 printf("Connection established with the host: %s\n", sWUHost);
	 
	 iTotal = strlen(sRequest);
	 iSent = 0;
	 do
	 {
		 iBytesSent = write(iSocketFD,sRequest+iSent,iTotal-iSent);
		 printf("%d\n",iBytesSent);
		 if(iBytesSent < 0)
		 {
			 printf("ERROR: The message can't be written in the socket.\n");
			 return SEND_DATA_ERROR;
		 }
		 if(iBytesSent == 0)
		 {
			 break;
		 }
		 iSent += iBytesSent;
	 }while(iSent < iTotal);
	 printf("Message sent.\n%s\n",sRequest);
	 send(iSocketFD,sRequest,strlen(sRequest),0);
	 printf("Second message sent.\n");
	 
	 //shutdown(iSocketFD,SHUT_WR);
	 select(iSocketFD+1,&desLectura,NULL,NULL,NULL);
	 
	 iTotal = strlen(sResponse);
	 iReceived = 0;
	 do
	 {
		 iBytesReceived = read(iSocketFD,sResponse+iReceived,iTotal-iReceived);
		 printf("%d\n",iBytesReceived);
		 if(iBytesReceived < 0)
		 {
			 printf("ERROR: The response can't be read from the socket.\n");
			 return SEND_DATA_ERROR;
		 }
		 if(iBytesReceived == 0)
		 {
			 break;
		 }
		 iReceived += iBytesReceived;
	 }while(iReceived < iTotal);
	 printf("Response received.\n");
	 
	 if(iReceived == iTotal)
	 {
		 printf("ERROR: The response is bigger than the buffer.\n");
	 }
	 
	 iReceived = recv(iSocketFD,sResponse,strlen(sResponse),0);
	 if(iReceived == iTotal)
	 {
		 printf("ERROR: The response is bigger than the buffer.\n%d\n",iReceived);
	 }
	 
	 close(iSocketFD);
	 printf("Connection closed.\n");
	 printf("Response: %s\n", sResponse);
	 return 0;
 }
 
 int main(int argc, char *argv[])
 {
	 printf("Variables declaration...");
	 char sUrlMessage[BUFFER_NET_LEN];
	 printf(" OK\n");
	 
	 printf("Variables initialization...");
	 memcpy(sUrlMessage,"\0",strlen(sUrlMessage));
	 sprintf(sUrlMessage,"ID=ICOMUNID85&PASSWORD=1142aee1&action=updateraw&dateutc=now&humidity=80&baromin=2992&tempf=55");
	 printf(" OK\n");
	 
	 if(iSendDataToWUService(sUrlMessage))
	 {
		 printf("ERROR: Failure in the communication with the Weather Underground services to send the data.\n");
	 }else
	 {
		 printf("OK: Data sent to Weather Underground\n");
	 }
	 return 0;
 } 
