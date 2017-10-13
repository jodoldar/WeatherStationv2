#ifndef TE923DRIVER_H_
#define TE923DRIVER_H_

#define VERSION "0.6.1"
#define TRUE 1
#define FALSE 0

typedef struct {
	// this are the values from station
	unsigned int timestamp;                 // timestamp for this dataset
	float t[6];                             // temperature of the sensors in °C
	signed short h[6];                      // humidity of the sensors in % rel
	float uv;                               // UV index
	float press;                            // air pressure in mBar
	unsigned char forecast;                 // weather forecast from station
											//                                      0 => heavy snow
											//                                      1 => snow
											//                                      2 => heavy rain
											//                                      3 => rain
											//                                      4 => cloudy
											//                                      5 => some clouds
											//                                      6 => sunny
	unsigned char storm;                    // storm warning if value = 1 else 0
	float wChill;                           // windchill in °C
	float wGust;                            // wind gusts in m/s
	float wSpeed;                           // wind speed in m/s
	unsigned char wDir;                     // wind direction in x*22.5°; 0 => north
	unsigned int RainCount;                 // Raincounter of station as number up to 65535 * 0.7 mm/m²
											// status of sensors, names are _<sensor>; if status is <> 0, the value should be 0
											// 0  => value of sensor should be ok
											// -1 => data is invalid
											// -2 => sensor is out of range
											// -3 => missing link
											// -4 => any other error
	signed char _t[6], _h[6];
	signed char _press, _uv, _wDir, _wSpeed, _wGust, _wChill, _RainCount, _storm, _forecast;
	unsigned int __src;                     // source adress of dataset (needed for dump)
} Te923DataSet_t;



typedef struct {
	unsigned char SysVer;                   // programversion system controller
	unsigned char BarVer;                   // programversion barometer
	unsigned char UvVer;                    // programversion UV sensor and ext. channels
	unsigned char RccVer;                   // programversion remote controlled clock and sunset 
	unsigned char WindVer;                  // programversion wind- rain sensor
	_Bool battery1 : 1;						// battery in sensor 1
	_Bool battery2 : 1;						// battery in sensor 2
	_Bool battery3 : 1;						// battery in sensor 3
	_Bool battery4 : 1;						// battery in sensor 4
	_Bool battery5 : 1;						// battery in sensor 5
	_Bool batteryUV : 1;					// battery in UV sensor
	_Bool batteryWind : 1;                  // battery in wind sensor
	_Bool batteryRain : 1;                  // battery in rain sensor
} Te923DevSet_t;

void printBuffer(unsigned char *buf, int size);
int bcd2int(char bcd);
int read_from_te923(usb_dev_handle *devh, int adr, unsigned char rbuf[BUFLEN]);
int decode_te923_data(unsigned char buf[], Te923DataSet_t *data);
int get_te923_memdata(usb_dev_handle *devh, Te923DataSet_t *data, unsigned short bigmem);
int get_te923_lifedata(usb_dev_handle *devh, Te923DataSet_t *data);
int get_te923_devstate(usb_dev_handle *devh, Te923DevSet_t *dev);

#endif /* TE923DRIVER_H_ */
