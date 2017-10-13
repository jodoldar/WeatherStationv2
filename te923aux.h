#ifndef TE923AUX_H_
#define TE923AUX_H_

#include <usb.h>

#define TE923_VENDOR    0x1130
#define TE923_PRODUCT   0x6801
#define BUFLEN          35


//extern unsigned short debug;

void usage();
void printData(Te923DataSet_t *data, char *iText);
struct usb_device *find_te923();
struct usb_dev_handle *te923_handle();
void te923_close(usb_dev_handle *devh);

#endif  /* TE923USB_H_ */
