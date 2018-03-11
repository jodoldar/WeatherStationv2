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
 
 #include <math.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <sys/select.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <usb.h>
 #include "WeatherStat.h"
 #include "te923aux.h"
 #include "te923driver.h"
 
  int main(int argc, char *argv[])
 {
	sleep(60); //Esperamos 1 minuto para que arranque el sistema
	// Lanzamos el bot de telegram
	int pid, ret;
	
	pid = fork();
	
	if (pid == 0)
	{
		// Soy el proceso hijo
		ret = execl ("/home/pi/METARBot/MetarBot.py","/home/pi/METARBot/MetarBot.py", (char*) NULL);
		return ret;
	}
	else
	{
		//Soy el proceso original
		do{
			printf("Variables declaration...");
			char sUrlMessage[BUFFER_NET_LEN];
			char sParsedMessage[BUFFER_NET_LEN];
			struct usb_dev_handle *stUsbDevice;
			Te923DataSet_t *stDataSet;
			printf(" OK\n");
		 
			printf("Variables initialization...");
			memcpy(sUrlMessage,"\0",strlen(sUrlMessage));
			memcpy(sParsedMessage,"\0",strlen(sParsedMessage));
			stUsbDevice = (usb_dev_handle*)malloc(sizeof(stUsbDevice));
			memset(stUsbDevice,0,sizeof(stUsbDevice));
			stDataSet = (Te923DataSet_t*)malloc(sizeof(Te923DataSet_t));
			memset(stDataSet,0,sizeof(Te923DataSet_t));
			printf(" OK\n");
		 
			// Retrieve information from the device
			printf("Retrieving information from the device...");
			stUsbDevice = te923_handle();
			get_te923_lifedata(stUsbDevice,stDataSet);
			te923_close(stUsbDevice);
			printf(" OK\n");

			// Create the URL with the data received
			printf("Creating an URL with information from the device...");
			sprintf(sUrlMessage,"ID=ICOMUNID85&PASSWORD=1142aee1&action=updateraw&dateutc=now&rainin=0&dailyrainin=0"); //&humidity=80&baromin=2992&tempf=55");
			vCreateUrlFromData(stDataSet, sUrlMessage);
			printf(" OK \n");

			// Envio de la informacion a Weather Underground
			printf("Sending information to Weather Underground... ");
			if(iSendDataToWUService(sUrlMessage))
			{
				perror("ERROR: Failure in the communication with the Weather Underground services to send the data.\n");
				printf(" FAIL\n");
			}else
			{
				printf(" OK\n"); //Data sent to Weather Underground\n");
			}

			printf("Preparing string for the local server...");
			dataToString(stDataSet,sParsedMessage);
			printf(" OK\n");

			int iSocketID = 0;
			printf("Sending information to local server...");
			if(iSendDataToLocalServer(sParsedMessage, &iSocketID))
			{
				if(iSocketID != 0){
					fprintf(stderr,"Socket id: %d\n", iSocketID);
					close(iSocketID);
					perror("Socket closed from outside");
				}
				perror("ERROR: Failure in the communication with the local server.\n");
				printf(" FAIL\n");
			}else
			{
				printf(" OK\n"); //Data sent to the local server
			}

			sleep(60);
			//sleep(300);
		}while(TRUE);
		return 0;
	}
 } 
 
 void vCreateUrlFromData(Te923DataSet_t *stInputDataSet, char *sOutputUrl)
 {
	int i;
	float dewpt;
	char sAuxElem[BUFFER_NET_LEN];

	memcpy(sAuxElem,"\0",strlen(sAuxElem));
	dewpt = 0;

	if(stInputDataSet->_t[1]==0){
		sprintf(sAuxElem,"&tempf=%0.2f",((stInputDataSet->t[1]*9)/5)+32);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_h[1]==0){
		sprintf(sAuxElem,"&humidity=%d",stInputDataSet->h[1]);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_press==0){
		sprintf(sAuxElem,"&baromin=%0.2f",stInputDataSet->press*0.02952998751);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_wSpeed==0){
		sprintf(sAuxElem,"&windspeedmph=%0.1f",stInputDataSet->wSpeed*2.23694);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_wGust==0){
		sprintf(sAuxElem,"&windgustmph=%0.1f",stInputDataSet->wGust*2.23694);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_wDir==0){
		sprintf(sAuxElem,"&winddir=%0.0f",stInputDataSet->wDir*22.5);
		strcat(sOutputUrl,sAuxElem);
	}
	if(stInputDataSet->_t[1]==0 && stInputDataSet->_h[1]==0){
		dewpt = pow(((float)stInputDataSet->h[1]/100),0.125);
		dewpt *= (112+(0.9*stInputDataSet->t[1]));
		dewpt += (stInputDataSet->t[1]*0.1);
		dewpt -= 112;
		sprintf(sAuxElem,"&dewptf=%0.2f",((dewpt*9)/5)+32);
		strcat(sOutputUrl,sAuxElem);
	}
	 
	if(DEBUG_PARSE){
		 printf("Information: \n");
		 printf("Timestamp: %u\n", stInputDataSet->timestamp);
		 for(i=0;i<6;i++){
			 if(stInputDataSet->_t[i]==0){
				printf("Temperature[%d]: %0.2f\n",i, stInputDataSet->t[i]);
			}
		 }
		 for(i=0;i<6;i++){
			 if(stInputDataSet->_h[i]==0){
				printf("Humidity[%d]: %d\n",i, stInputDataSet->h[i]);
			}
			
		 }
		 printf("Wind chill: %0.1f\n", stInputDataSet->wChill);
		 printf("Wind speed: %0.1f\n", stInputDataSet->wSpeed);
		 printf("Wind gust: %0.1f\n", stInputDataSet->wGust);
		 printf("Wind dir: %0.0f\n", stInputDataSet->wDir*22.5);
		 printf("Pressure: %0.1f\n", stInputDataSet->press);
		 printf("Punto de rocio: %0.2f\n", dewpt);
		 printData(stInputDataSet,":");
	}
	 
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
 
 int iSendDataToLocalServer(char *sMessage, int *iSocketFD){
	 int iPortNumber;
	 char sServerHost[BUFFER_NET_LEN];
	 struct hostent *stServer;
	 struct sockaddr_in stServ_addr;
	 int iBytesSent, iBytesReceived;
	 int iSent, iReceived;
	 int iTotal;
	 char sRequest[BUFFER_NET_LEN];
	 char sResponse[BUFFER_NET_LEN];
	 
	 iPortNumber = 11000;
	 memcpy(sServerHost,"\0",strlen(sServerHost));
	 memcpy(sRequest,"\0",strlen(sRequest));
	 iBytesSent = 0;
	 iBytesReceived = 0;
	 iSent = 0;
	 iReceived = 0;
	 iTotal = 0;
	 memcpy(sResponse,"\0",strlen(sResponse));
	 
	 sprintf(sServerHost,"192.168.0.158");
	 sprintf(sRequest,"%s<EOF>",sMessage);
	 
	 if(DEBUG_NET){
		printf("Request:\n%s\n",sRequest);
		printf("Longitude: %d\n",strlen(sRequest));
	 }
	 
	 *iSocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	 if(DEBUG_NET){
		printf("Socket created\n");
	}
	 if(*iSocketFD < 0)
	 {
		perror("ERROR: Fail at opening the socket.\n");
		 return SEND_DATA_ERROR;
	 }
	 
	 stServer = gethostbyname(sServerHost);
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
	 
	 if(connect(*iSocketFD,(struct sockaddr *)&stServ_addr,sizeof(stServ_addr)) < 0)
	 {
		 perror("ERROR: The connection can't be established with the host.\n");
		 return SEND_DATA_ERROR;
	 }
	 if(DEBUG_NET){
		printf("Connection established with the host: %s\n", sServerHost);
	}
	 
	 iTotal = strlen(sRequest);
	 iSent = 0;
	 do
	 {
		 iBytesSent = write(*iSocketFD,sRequest+iSent,iTotal-iSent);
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
		 iBytesReceived = read(*iSocketFD,sResponse+iReceived,iTotal-iReceived);
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
	 
	 close(*iSocketFD);
	 if(DEBUG_NET){
		printf("Connection closed.\n");
		printf("Response: %s\n", sResponse);
	}
	
	if(strncmp(sResponse,sRequest,strlen(sRequest)) == 0){
		return 0;
	}else{
		return 1;
	}
 }
