/**
 * Project: Paper. 4 pin Fan http://chafalladas.com
 * Author: Alfonso Abelenda Escudero alfonso@abelenda.es
 * Inspired by V-USB example code by Christian Starkjohann and v-usb tutorials from http://codeandlife.com
 * Hardware based on tinyUSBboard http://matrixstorm.com/avr/tinyusbboard/ and paperduino perfboard from http://txapuzas.blogspot.com.es
 * Copyright: (C) 2017 Alfonso Abelenda Escudero
 * License: GNU GPL v3 (see License.txt)
 */

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <process.h>
#include <stdlib.h>

//Some control functions.
#include <dialog_functions.c>
#include <usb_functions.c>
#include <imported_functions.c>

// this is libusb, see http://libusb.sourceforge.net/

#include "resource.h"

// same as in main.c
#define USB_MOD_FAN 0
#define USB_WRITE_CONF 1
#define USB_READ_CONF  2
#define USB_RESET_CONF  3
#define USB_READ_SENS  4


unsigned char autofan = 4;
unsigned char pwm_01 = 128;
unsigned char pwm_02 = 128;
unsigned char p_pwm_01 = 128;
unsigned char p_pwm_02 = 128;
int SW_send_pwm = 0;
int tSleep = 1;

struct Params
{
    int param1;
    long long param2;
    HWND pa_hwndDlg;
    UINT pa_uMsg;
    WPARAM pa_wParam;
    LPARAM pa_lParam;
};
//HWND p_hwndDlg; //Defined in usb_functions
int P_statusline;
int P_statuslabel;
int P_myCommand;
char *P_statusMsg;
char *P_labelMsg;
char p_bytes[3];
bool P_etiqueta;
int collision = 0;
int sendCommands(HWND hwndDlg, int statusline, int statuslabel, int myCommand, char *statusMsg, char *labelMsg, bool etiqueta)
{
//usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
    int wmId, wmEvent; //Identifiers of control and events related with them
    int nBytes = 0; //Case to send to the USB device
    char entero[16];
    unsigned char mientero;
    char buffer[256]; //Buffer of the messages
    usb_dev_handle *handle = NULL;
    handle = usbOpenDevice(0x16C0, "chafalladas.com", 0x05DC, "usbPeltier");

    if(handle == NULL)
    {
        SetDlgItemText(hwndDlg, ID_STATUS1, "Could not find USB device! send command\n"); //(ID_STATUS1);
        return 0;
    }
    SetDlgItemText(hwndDlg, statusline, statusMsg); //Notify action in status bar;
    nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, myCommand, 0, 0, (char *)buffer, sizeof(buffer), 5000); //Send command to the vusibino
    usb_close(handle);
    if (etiqueta)
    {
        SetDlgItemText(hwndDlg, statuslabel, labelMsg); //Notify action in the checkbox info text
    }
    else
    {
        unsigned char  lsb; // signed 8-bit value
        unsigned char  msb; // signed 8-bit value
        unsigned short combined = msb << 8  |  (lsb & 0xFF); // signed 16-bit value

        msb=(char)buffer[0];
        lsb=(char)buffer[1];
        pwm_01 = lsb;
        if(autofan & 1)
            redrawSlider(hwndDlg, ID_TF_1, ID_BUDDY_FAN1, pwm_01, PERCENTAGE);
        combined = msb << 8  |  (lsb & 0xFF);
        combined  = (combined  * 100)/255;
        sprintf(entero, "Duty 1: %i%c", combined, 0x25);
        SetDlgItemText(hwndDlg, ID_FAN_PWM_1, entero);
        msb=(char)buffer[4];
        lsb=(char)buffer[5];
        combined = msb << 8  |  (lsb & 0xFF);
        sprintf(entero, "Fan 1: %i RPM", combined);
        SetDlgItemText(hwndDlg, ID_FAN_RPM_1, entero);
        msb=(char)buffer[2];
        lsb=(char)buffer[3];
        combined = msb << 8  |  (lsb & 0xFF);
        combined  = (combined  * 100)/255;
        pwm_02 = lsb;
        if(autofan & 2)
            redrawSlider(hwndDlg, ID_TF_2, ID_BUDDY_FAN2, pwm_02, PERCENTAGE);
        sprintf(entero, "Duty 2: %i%c", combined, 0x25);
        SetDlgItemText(hwndDlg, ID_FAN_PWM_2, entero);
        msb=(char)buffer[6];
        lsb=(char)buffer[7];
        combined = msb << 8  |  (lsb & 0xFF);
        sprintf(entero, "Fan 2: %i RPM", combined);
        SetDlgItemText(hwndDlg, ID_FAN_RPM_2, entero);
    }
    return 0;
}

//Open dialogs
HINSTANCE hInst;
//Function for Main dialog
BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent; //Identifiers of control and eventes related with them
    int nBytes = 0; //Case to send to the USB device
    char buffer[256], strInfo[256]; //Buffer of the messages
    MSG msg;
    usb_dev_handle *handle = NULL;
// Posibly the worst aproximation, but works.
    p_hwndDlg = hwndDlg;
    P_statusline = ID_STATUS1;
    P_statuslabel = ID_FAN_PWM_1;
    P_myCommand = USB_READ_SENS;
    P_statusMsg = "Message from thread -=============================-";
    P_etiqueta = false;
printf("UNO");
    switch(uMsg)
    {
    case WM_INITDIALOG: //Initialize dialog
    {
        #define USB_CFG_DEVICE_NAME_LEN 10
/* Same as above for the device name. If you don't want a device name, undefine
* the macros. See the file USB-IDs-for-free.txt before you assign a name if
* you use a shared VID/PID.
*/
// Serial  PLT001
        handle = usbOpenDevice(0x16C0, "chafalladas.com", 0x05DC, "usbPeltier");
        if(handle == NULL)
        {
            SetDlgItemText(hwndDlg, ID_STATUS1, "Could not find USB device!\n"); //(ID_STATUS1);
            break;
        }
        else
        {
            SetDlgItemText(hwndDlg, ID_STATUS1, "Found 0x16C0 - chafalladas.com - 0x05DC - usbPeltier"); //(ID_STATUS1);
            SetDlgItemText(hwndDlg, ID_DEVICE, "Device: 0x16C0 - chafalladas.com - 0x05DC - usbPeltier"); //(ID_STATUS1);
            sprintf(strInfo, "Serial number: %s", usbSerial); //Format the log line
            SetDlgItemText(hwndDlg, ID_SERIAL, strInfo); //(ID_STATUS1);
            usb_close(handle);
        }
        //Dirty Sync Done Dirt Bit.
        tSleep = tSleep | 8;  //This tells the threaded USB interactions thet there is something going on
        handle = usbOpenDevice(0x16C0, "chafalladas.com", 0x05DC, "usbPeltier");
        sprintf(strInfo, "Reading configuration"); //Format the log line
        nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, USB_READ_CONF, 0, 0, (char *)buffer, sizeof(buffer), 5000); //Send command to the vusibino
        p_pwm_01 = (char)buffer[1];
        p_pwm_02 = (char)buffer[3];
        autofan = (char)buffer[8];
        usb_close(handle);
        tSleep = tSleep & 0xF7;  //Liberate block
        printf("Checking confs: autofan=%u pwm_01=%u pwm_02=%u\n", autofan,p_pwm_01,p_pwm_02);
            if(autofan & 1)
            {
                SendMessage(GetDlgItem(hwndDlg, ID_CHK_AUTO1), BM_SETCHECK, BST_CHECKED, 0);
            }
            if(autofan & 2)
            {
                SendMessage(GetDlgItem(hwndDlg, ID_CHK_AUTO2), BM_SETCHECK, BST_CHECKED, 0);
            }
//        initialize Sliders
        setMySlider(hwndDlg, ID_TF_1, 1, 255, 32, p_pwm_01, ID_BUDDY_FAN1, PERCENTAGE); //Declared in dialog_functions.c
        setMySlider(hwndDlg, ID_TF_2, 1, 255, 32, p_pwm_02, ID_BUDDY_FAN2, PERCENTAGE);
//Put an icon on the dialog window
        HICON hIcon;
        hIcon = (HICON)LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        if (hIcon)
        {
            SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }
    }
    return TRUE;

    case WM_CLOSE: //Close Dialog
    {
        EndDialog(hwndDlg, 0);
    }
    return TRUE;

    case WM_COMMAND: //Various commands processing
    {
    time_t now = time(0);       //Get time of the event
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

        wmId = LOWORD(wParam);   //These look kewl I'll save them for later
        wmEvent = HIWORD(wParam);//These look kewl I'll save them for later
        switch(LOWORD(wParam))
        {
        case ID_CLOSE: //Close
        {
            EndDialog(hwndDlg, 0);
        }
        return TRUE;
        case ID_SAVE: //Save the setting to eeprom
        {
            sprintf(strInfo, "%s;Values saved in eeprom\r\n", buf); //Format the log line
                                    //Dirty Sync Done Dirt Bit.
            tSleep = tSleep | 8;  //This tells the threaded USB interactions thet there is something going on
            sendCommands(p_hwndDlg , P_statusline, P_statuslabel, USB_WRITE_CONF , strInfo ,"",false ); //Send the command
            appendLogText(hwndDlg, ID_LOG, strInfo);
            tSleep = tSleep & 0xF7;  //Liberate block
        }
        return TRUE;
        case ID_RESET: //Save the setting to eeprom
        {
            sprintf(strInfo, "%s;Values resseted in eeprom\r\n", buf); //Format the log line
                                    //Dirty Sync Done Dirt Bit.
            tSleep = tSleep | 8;  //This tells the threaded USB interactions thet there is something going on
            sendCommands(p_hwndDlg , P_statusline, P_statuslabel, USB_RESET_CONF , strInfo ,"",false ); //Send the command
            appendLogText(hwndDlg, ID_LOG, strInfo);
            tSleep = tSleep & 0xF7;  //Liberate block
        }
        return TRUE;
        case ID_CHK_AUTO1:
        {
            if (wmEvent == BN_CLICKED) //Is checkbox clicked?
            {
                LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0); //Which is the status?
                if (chkState == BST_CHECKED)
                {
                    sprintf(strInfo, "%s;Checkbox Auto fan 1 Checked -===========================-\r\n", buf); //Format the log line
                    appendLogText(hwndDlg, ID_LOG, strInfo);
                    autofan = autofan | 1;

                } else
                {
                    sprintf(strInfo, "%s;Checkbox Auto fan 1 Unchecked -===========================-\r\n", buf); //Format the log line
                    appendLogText(hwndDlg, ID_LOG, strInfo);
                    autofan = autofan & 0xFE;
                }
            }
        }
        return TRUE;
        case ID_CHK_AUTO2:
        {
            if (wmEvent == BN_CLICKED) //Is checkbox clicked?
            {
                LRESULT chkState = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0); //Which is the status?
                if (chkState == BST_CHECKED)
                {
                    sprintf(strInfo, "%s;Checkbox Auto fan 2 Checked -===========================-\r\n", buf); //Format the log line
                    appendLogText(hwndDlg, ID_LOG, strInfo);
                    autofan = autofan | 2;

                } else
                {
                    sprintf(strInfo, "%s;Checkbox Auto fan 2 Unchecked -===========================-\r\n", buf); //Format the log line
                    appendLogText(hwndDlg, ID_LOG, strInfo);
                    autofan = autofan & 0xFD;
                }
            }
        }
        return TRUE;
        }
    }
    case WM_HSCROLL:
        {
        wmId = LOWORD(wParam);   //These look kewl I'll save them for later
        wmEvent = HIWORD(wParam);//These look kewl I'll save them for later

        p_pwm_01 = setInfoSlider(hwndDlg, ID_TF_1, ID_BUDDY_FAN1, PERCENTAGE);
        p_pwm_02 = setInfoSlider(hwndDlg, ID_TF_2, ID_BUDDY_FAN2, PERCENTAGE);

        LRESULT chkState = SendMessage(GetDlgItem(hwndDlg ,ID_CHK_AUTO1), BM_GETCHECK, 0, 0); //Which is the status?
        if (chkState == BST_UNCHECKED)
            {
                if((wmId == TB_THUMBPOSITION) || (wmId == TB_ENDTRACK))
                {
                    printf("PWM 1 %i\n", p_pwm_01 );
                }
            }
        chkState = SendMessage(GetDlgItem(hwndDlg ,ID_CHK_AUTO2), BM_GETCHECK, 0, 0); //Which is the status?
        if (chkState == BST_UNCHECKED)
            {
                if((wmId == TB_THUMBPOSITION) || (wmId == TB_ENDTRACK))
                {
                    printf("PWM 2 %i\n", p_pwm_02 );
                }
            }
        }

    return TRUE;
    }
    return FALSE;

}

void wTest(void *p)
{
    for(;;)
    {
        tSleep = tSleep | 2; //Dirty Sync Done Dirt Bit
        Sleep( 500L );
        tSleep = tSleep & 0xFD;
//        itoa (autofan,buffer,2);
        if (tSleep == 1) //Is sendPwm sleeping and no other comms with the Device active?
            sendCommands(p_hwndDlg , P_statusline, P_statuslabel, USB_READ_SENS , P_statusMsg ,"",P_etiqueta );
        else if(tSleep == 0)
            collision++;
    }
}

void SendPwm( void * parg )
{
    for(;;)
    {
        tSleep = tSleep | 1;
        Sleep( 1001L );
        tSleep = tSleep & 0xFE;
        p_bytes[0] = autofan;
        p_bytes[1] = p_pwm_01;
        p_bytes[2] = p_pwm_02;
        if (tSleep == 2) //Is wTest sleeping and no other comms with the Device active?
        {
            if(autofan == 7)
            {
                sendBytes(p_hwndDlg, P_statusline, P_statuslabel, USB_MOD_FAN, P_statusMsg, p_bytes);
            }
            if (autofan == 5)
            {
                sendBytes(p_hwndDlg, P_statusline, P_statuslabel, USB_MOD_FAN, P_statusMsg, p_bytes);
            }
            if (autofan == 6)
            {
                sendBytes(p_hwndDlg, P_statusline, P_statuslabel, USB_MOD_FAN, P_statusMsg, p_bytes);
            }
            if (autofan == 4)
            {
                sendBytes(p_hwndDlg, P_statusline, P_statuslabel, USB_MOD_FAN, P_statusMsg, p_bytes);
            }
        }
        else if(tSleep == 0)
            collision++;
    }
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    int param = 0;
    int * pparam = &param;
    _beginthread( SendPwm, 0, (void *) pparam );
    _beginthread( wTest, 0, (void *) pparam );
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);

}
//Thats all, a rework may strip lots of unnecessary lines. Maybe tomorrow.
