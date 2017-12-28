#include <stdio.h>
#include <usb.h>
#include <time.h>
#include <stdbool.h>

#include "resource.h"
HWND p_hwndDlg;
char usbSerial[256];

// used to get descriptor strings for device identification
static int usbGetDescriptorString(usb_dev_handle *dev, int index, int langid, char *buf, int buflen)
{
    char buffer[256];
    int rval, i;
    // make standard request GET_DESCRIPTOR, type string and given index
    // (e.g. dev->iProduct)
    rval = usb_control_msg(dev, USB_TYPE_STANDARD | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, langid, buffer, sizeof(buffer), 1000);

    if(rval < 0) // error
        return rval;

    // rval should be bytes read, but buffer[0] contains the actual response size
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0]; // string is shorter than bytes read

    if(buffer[1] != USB_DT_STRING) // second byte is the data type
        return 0; // invalid return type

    // we're dealing with UTF-16LE here so actual chars is half of rval,
    // and index 0 doesn't count
    rval /= 2;

    // lossy conversion to ISO Latin1
    for(i = 1; i < rval && i < buflen; i++)
    {
        if(buffer[2 * i + 1] == 0)
            buf[i-1] = buffer[2 * i];
        else
            buf[i-1] = '?'; // outside of ISO Latin1 range
    }
    buf[i-1] = 0;

    return i-1;
}


static usb_dev_handle * usbOpenDevice(int vendor, char *vendorName, int product, char *productName)
{

    struct usb_bus *bus;
    struct usb_device *dev;
    char devVendor[256], devProduct[256], serial[256];
    char strInfo[256]; //Only necessary to interact with dialog
    time_t now = time(0);       //Get time of the event
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    int len;
    usb_dev_handle * handle = NULL;
    //fprintf(stderr, "usb_dev_handle: %s\n", productName);
    usb_init();
    usb_find_busses();
    usb_find_devices();

    for(bus=usb_get_busses(); bus; bus=bus->next)
    {
        for(dev=bus->devices; dev; dev=dev->next)
        {
            if(dev->descriptor.idVendor != vendor || dev->descriptor.idProduct != product)
                continue;

            // we need to open the device in order to query strings
            if(!(handle = usb_open(dev)))
            {
                fprintf(stderr, "Warning: cannot open USB device: %s\n", usb_strerror());
                sprintf(strInfo, "%s;Warning: cannot open USB device: %s\r\n", buf,  usb_strerror());
                appendLogText(p_hwndDlg, ID_LOG, strInfo);
                usb_close(handle);
                continue;
            }

            // get vendor name
            if(usbGetDescriptorString(handle, dev->descriptor.iManufacturer, 0x0409, devVendor, sizeof(devVendor)) < 0)
            {
                fprintf(stderr, "Warning: cannot query manufacturer for device: %s\n", usb_strerror());
                sprintf(strInfo, "%s;Warning: cannot query manufacturer for device: %s\r\n", buf, usb_strerror());
                appendLogText(p_hwndDlg, ID_LOG, strInfo);
                usb_close(handle);
                continue;
            }

            // get product name
            if(usbGetDescriptorString(handle, dev->descriptor.iProduct, 0x0409, devProduct, sizeof(devVendor)) < 0)
            {
                fprintf(stderr, "Warning: cannot query product for device: %s\n", usb_strerror());
                sprintf(strInfo, "%s;Warning: cannot query product for device: %s\r\n", buf, usb_strerror());
                appendLogText(p_hwndDlg, ID_LOG, strInfo);
                usb_close(handle);
                continue;
            }
            if(dev->descriptor.iSerialNumber > 0)
            {
                len = usbGetDescriptorString(handle, dev->descriptor.iSerialNumber,0x0409, usbSerial, sizeof(usbSerial));
            }

            if(strcmp(devVendor, vendorName) == 0 && strcmp(devProduct, productName) == 0)
                return handle;
            else
                usb_close(handle);
        }
    }
    return NULL;
}

int sendBytes(HWND hwndDlg, int statusline, int statuslabel, int myCommand, char *statusMsg, char *bytesSent)
{
//usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
    int wmId, wmEvent; //Identifiers of control and events related with them
    int nBytes = 0; //Case to send to the USB device
    char entero[16];
    unsigned char mientero;
    char buffer[256]; //Buffer of the messages
    usb_dev_handle *handle = NULL;
    char strInfo[256]; //Only necessary to interact with dialog

    time_t now = time(0);       //Get time of the event
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    handle = usbOpenDevice(0x16C0, "chafalladas.com", 0x05DC, "usbPeltier");
    if(handle == NULL)
    {
        SetDlgItemText(hwndDlg, ID_STATUS1, "Could not find USB device!\n"); //(ID_STATUS1);
        sprintf(strInfo, "%s;Could not find USB device! sending command %i\r\n", buf, myCommand); //Format the log line
        appendLogText(hwndDlg, ID_LOG, strInfo);
        return 0;
    }
    printf("Sending PWM %d %d %d\n", bytesSent[0],bytesSent[1],bytesSent[2]);
    SetDlgItemText(hwndDlg, statusline, statusMsg); //Notify action in status bar;
    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, myCommand, 0, 0, bytesSent, strlen(bytesSent)+1, 5000); //Send command to the vusibino
    usb_close(handle);
    return 0;
}
