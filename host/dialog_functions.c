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

//We define what units we will display in a slider buddy. Only one will be used in this code
#define PERCENTAGE 0
#define CELSIUS 1
#define FARENHEIT 2
#define RPM 3
#define HERTZ 4
#define VOLTS 5
#define MSECS 6
#define CSECS 7
#define SECS 8
#define MINS 9
#define AS_IS 10

//Put formatted slider values in a buddy box. The function returns the position value
int setInfoSlider(HWND hwndDlg,int sliderId, int buddyId, int sliderUnits)
{
    int wmId, wmEvent; //Identifiers of control and events related with them
    int nBytes = 0; //Case to send to the USB device

    int Pos, percent, percentile;
    char szPos[16];
    switch(sliderUnits)
    {
    case PERCENTAGE:
        {
            Pos = SendMessage(GetDlgItem(hwndDlg, sliderId), TBM_GETPOS, 0, 0);
            percentile = SendMessage(GetDlgItem(hwndDlg, sliderId), TBM_GETRANGEMAX, 0, 0);
            percent = (Pos * 100)/percentile;
            sprintf(szPos, "%i %c", percent, 0x25);
            SetDlgItemText(hwndDlg, buddyId, szPos);
        }
        return Pos;
    case AS_IS:
        {
            Pos = SendMessage(GetDlgItem(hwndDlg, sliderId), TBM_GETPOS, 0, 0);
            sprintf(szPos, "%i", Pos);
            SetDlgItemText(hwndDlg, buddyId, szPos);
        }
        return Pos;
    }
    return FALSE;
}

//Put formatted usb returned values in a text box.
int setUSBInfo(HWND hwndDlg,int infoBox, int infoUnits)
{

return 0;
}

void appendLogText(HWND hwndDlg, int infoBox, LPCTSTR newText)
{
//Log snippet based on these answers
//https://stackoverflow.com/questions/7200137/appending-text-to-a-win32-editbox-in-c

    int left,right;
    //int len = GetWindowTextLength(hwndDlg, infoBox);
    int len = SendMessage(GetDlgItem(hwndDlg, infoBox), EM_GETSEL,(WPARAM)&left,(LPARAM)&right);
    //Set the focus on the text edit control.
    SetFocus( GetDlgItem(hwndDlg, infoBox));
    // Simulate ctrl+page down to put the cursor at the bottom the box.
    HWND temp = GetActiveWindow(); //Prevent the keystrokes to be sent out of the dialog.
    if (temp == hwndDlg) // Then your current window has focus
    {
        keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
        keybd_event( VK_END, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
    // Simulate a key release
    keybd_event( VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    keybd_event( VK_END, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    //Enter the text to the log at the end of the textbox
    SendMessage(GetDlgItem(hwndDlg, infoBox), EM_GETSEL,(WPARAM)&left,(LPARAM)&right);
    SendMessage(GetDlgItem(hwndDlg, infoBox), EM_SETSEL, len, len);
    SendMessage(GetDlgItem(hwndDlg, infoBox), EM_REPLACESEL, 0, (LPARAM)newText);
    SendMessage(GetDlgItem(hwndDlg, infoBox), EM_SETSEL,left,right);
}

redrawSlider(HWND hwndDlg,int sliderId, int buddyId, int newValue, int sliderUnits)
{
    SendMessage(GetDlgItem(hwndDlg, sliderId), TBM_SETPOS, 1, newValue); //Initialize slider
    setInfoSlider(hwndDlg, sliderId, buddyId, sliderUnits); //Put the value of the slider in a buddy.
}

//Initialize slider
void setMySlider(HWND hwndDlg, int sliderId, int minRange, int maxRange, int ticFreq, int initPos, int buddyID, int sliderUnits)
{
    SendMessage (GetDlgItem(hwndDlg ,sliderId),TBM_SETRANGE,1,MAKELONG(minRange, maxRange)); //Set range of slider
    SendMessage (GetDlgItem(hwndDlg ,sliderId),TBM_SETTICFREQ,ticFreq,0); //Put tickers in the Sliders
    redrawSlider(hwndDlg, sliderId, buddyID, initPos, sliderUnits);
}
