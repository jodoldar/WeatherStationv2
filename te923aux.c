#include "te923aux.h"
#include <stdio.h>
#include <string.h>

void usage() {
	printf("te923con, connector for TE923 weatherstations v%s - (c) 2010 - 2013 by Sebastian John (te923@fukz.org)\n", VERSION);
	printf("This program is free software, distributed under the terms of the GNU General Public Licens.\n\n");
	printf("usage: te923con [options]\n");
	printf("  -D - Debug output (please send this output for bugfixing)\n");
	printf("  -d - dump all values from internal (small) memory\n");
	printf("  -b - dump all values from internal (big) memory (new hardware version)\n");
	printf("  -s - get status from weather station\n");
	printf("  -i - set the output for invalid values (unreachable sensors), default it is i\n");
	printf("  -h - print this help informations\n");
	printf("  -v - print version number of this program\n");
	printf("\n");
	printf("Output format is colon separatet values. Invalid values or values from not present sensors\n");
	printf("are hidden by default. You can set output for invalid with -i option. Output format:\n\n");
	printf("T0:H0:T1:H1:T2:H2:T3:H3:T4:H4:T5:H5:PRESS:UV:FC:STORM:WD:WS:WG:WC:RC\n\n");
	printf("  T0    - temperature from internal sensor in °C\n");
	printf("  H0    - humidity from internal sensor in %% rel\n");
	printf("  T1..5 - temperature from external sensor 1..4 in °C\n");
	printf("  H1..5 - humidity from external sensor 1...4 in %% rel\n");
	printf("  PRESS - air pressure in mBar\n");
	printf("  UV    - UV index from UV sensor\n");
	printf("  FC    - station forecast, see below for more details\n");
	printf("  STORM - stormwarning; 0 - no warning, 1 - fix your dog \n");
	printf("  WD    - wind direction in n x 22.5°; 0 -> north\n");
	printf("  WS    - wind speed in m/s\n");
	printf("  WG    - wind gust speed in m/s\n");
	printf("  WC    - windchill temperature in °C\n");
	printf("  RC    - rain counter (maybe since station starts measurement) as value\n");
	printf("\n");
	printf("    weather forecast means (as precisely as possible)\n");
	printf("      0 - heavy snow\n");
	printf("      1 - little snow\n");
	printf("      2 - heavy rain\n");
	printf("      3 - little rain\n");
	printf("      4 - cloudy\n");
	printf("      5 - some clouds\n");
	printf("      6 - sunny\n");
	printf("\n");
	printf("Status output means:\n\n");
	printf("SYSSW:BARSW:EXTSW:RCCSW:WINSW:BATR:BATU:BATW:BAT5:BAT4:BAT5:BAT2:BAT1\n\n");
	printf("  SYSSW  - software version of system controller\n");
	printf("  BARSW  - software version of barometer\n");
	printf("  EXTSW  - software version of UV and channel controller\n");
	printf("  RCCSW  - software version of rain controller\n");
	printf("  WINSW  - software version of wind controller\n");
	printf("  BATR   - battery of rain sensor (1-good (not present), 0-low)\n");
	printf("  BATU   - battery of UV sensor (1-good (not present), 0-low)\n");
	printf("  BATW   - battery of wind sensor (1-good (not present), 0-low)\n");
	printf("  BAT5   - battery of sensor 5 (1-good (not present), 0-low)\n");
	printf("  BAT4   - battery of sensor 4 (1-good (not present), 0-low)\n");
	printf("  BAT3   - battery of sensor 3 (1-good (not present), 0-low)\n");
	printf("  BAT2   - battery of sensor 2 (1-good (not present), 0-low)\n");
	printf("  BAT1   - battery of sensor 1 (1-good (not present), 0-low)\n");
	printf("\n");
	printf("For updates, bugfixes and support visit http://te923.fukz.org.\n");
}

void dataToString(Te923DataSet_t *data, char *output){
	int i;
	char auxiliar[BUFFER_STR_LEN];
	
	memcpy(auxiliar,"\0",strlen(auxiliar));
	memcpy(&output,"\0",strlen(output));
	
	sprintf(auxiliar,"%d:", data->timestamp);
	strcpy(output, auxiliar);
	
	for(i=0; i<=5; i++){
		if(data->_t[i] == 0){
			sprintf(auxiliar,"%0.2f:", data->t[i]);
			strcat(output, auxiliar);
		}else{
			sprintf(auxiliar,"%s:", "X");
			strcat(output, auxiliar);
		}
		
		if(data->_h[i] == 0){
			sprintf(auxiliar,"%d:", data->h[i]);
			strcat(output, auxiliar);
		}else{
			sprintf(auxiliar,"%s:", "X");
			strcat(output, auxiliar);
		}
	}
	
	if (data->_press == 0){
		sprintf(auxiliar, "%0.1f:", data->press);
		strcat(output, auxiliar);
	}else{
		sprintf(auxiliar, "%s:", "X");
		strcat(output, auxiliar);
	}
	
	if (data->_wDir == 0){
		sprintf(auxiliar,"%d:", data->wDir);
		strcat(output, auxiliar);
	}else{
		sprintf(auxiliar, "%s:", "X");
		strcat(output, auxiliar);
	}
	
	if (data->_wSpeed == 0){
		sprintf(auxiliar, "%0.1f:", data->wSpeed);
		strcat(output, auxiliar);
	}else{
		sprintf(auxiliar, "%s:", "X");
		strcat(output, auxiliar);
	}
	
	if (data->_wGust == 0){
		sprintf(auxiliar, "%0.1f:", data->wGust);
		strcat(output, auxiliar);
	}else{
		sprintf(auxiliar, "%s:", "X");
		strcat(output, auxiliar);
	}
	
	if (data->_wChill == 0){
		sprintf(auxiliar, "%0.1f:", data->wChill);
		strcat(output, auxiliar);
	}else{
		sprintf(auxiliar, "%s:", "X");
		strcat(output, auxiliar);
	}
}

void printData(Te923DataSet_t *data, char *iText) {
	int i;
	printf("%d:", data->timestamp);
	for (i = 0; i <= 5; i++) {

		if (data->_t[i] == 0)
			printf("%0.2f:", data->t[i]);
		else
			printf("%s:", iText);

		if (data->_h[i] == 0)
			printf("%d:", data->h[i]);
		else
			printf("%s:", iText);
	}

	if (data->_press == 0)
		printf("%0.1f:", data->press);
	else
		printf("%s:", iText);

	if (data->_uv == 0)
		printf("%0.1f:", data->uv);
	else
		printf("%s:", iText);

	if (data->_forecast == 0)
		printf("%d:", data->forecast);
	else
		printf("%s:", iText);

	if (data->_storm == 0)
		printf("%d:", data->storm);
	else
		printf("%s:", iText);

	if (data->_wDir == 0)
		printf("%d:", data->wDir);
	else
		printf("%s:", iText);

	if (data->_wSpeed == 0)
		printf("%0.1f:", data->wSpeed);
	else
		printf("%s:", iText);

	if (data->_wGust == 0)
		printf("%0.1f:", data->wGust);
	else
		printf("%s:", iText);

	if (data->_wChill == 0)
		printf("%0.1f:", data->wChill);
	else
		printf("%s:", iText);

	if (data->_RainCount == 0)
		printf("%d", data->RainCount);
	else
		printf("%s:", iText);

	printf("\n");
}

struct usb_device *find_te923() {
	usb_find_busses();
	usb_find_devices();
	struct usb_bus *bus;
	struct usb_device *dev;
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == TE923_VENDOR && dev->descriptor.idProduct == TE923_PRODUCT) {
				return dev;
			}
		}
	}
	return NULL;
}

struct usb_dev_handle *te923_handle() {
	int ret;
	int driver_name_len = 30;
	char driver_name[driver_name_len];
	struct usb_device *dev;
	struct usb_dev_handle *devh;

	usb_init();
	dev = find_te923();
	if (dev == NULL) {
		fprintf(stderr, "TE923 weather station cannot be found, sorry.\n");
		return NULL;
	}

	devh = usb_open(dev);
	if (devh == NULL) {
		fprintf(stderr, "Error while opening USB port and get device handler.\n");
		return NULL;
	}

	ret = usb_get_driver_np(devh, 0, driver_name, driver_name_len);
	if (ret == 0) {
		usb_detach_kernel_driver_np(devh, 0);
	}

	ret = usb_set_configuration(devh, 1);
	if (ret != 0) {
		fprintf(stderr, "Error while setting configuration (%d).\n", ret);
		return NULL;
	}

	ret = usb_claim_interface(devh, 0);
	if (ret != 0) {
		fprintf(stderr, "Error while claiming interface (%d).\n", ret);
		return NULL;
	}

	ret = usb_set_altinterface(devh, 0);
	if (ret != 0) {
		fprintf(stderr, "Error while setting alternative interface (%d).\n", ret);
		return NULL;
	}

	sleep(0.5);
	return devh;
}

void te923_close(usb_dev_handle *devh) {
	usb_release_interface(devh, 0);
	usb_close(devh);
}
