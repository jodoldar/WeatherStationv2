#ifndef TE923DRIVER_H_
#define TE923DRIVER_H_

#include "te923aux.h"

#define TRUE 1
#define FALSE 0


void printBuffer(unsigned char *buf, int size);
int bcd2int(char bcd);
int read_from_te923(usb_dev_handle *devh, int adr, unsigned char rbuf[BUFLEN]);
int decode_te923_data(unsigned char buf[], Te923DataSet_t *data);
int get_te923_memdata(usb_dev_handle *devh, Te923DataSet_t *data, unsigned short bigmem);
int get_te923_lifedata(usb_dev_handle *devh, Te923DataSet_t *data);
int get_te923_devstate(usb_dev_handle *devh, Te923DevSet_t *dev);

#endif /* TE923DRIVER_H_ */
