/**
 * Project: Paper. 4 pin Fan http://chafalladas.com
 * Author: Alfonso Abelenda Escudero alfonso@abelenda.es
 * Inspired by V-USB example code by Christian Starkjohann and v-usb tutorials from http://codeandlife.com
 * Hardware based on tinyUSBboard http://matrixstorm.com/avr/tinyusbboard/ and paperduino perfboard from http://txapuzas.blogspot.com.es
 * Copyright: (C) 2017 Alfonso Abelenda Escudero
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdbool.h>
#include "usbdrv.h"

//#define F_CPU 16000000L // uncomment if not defined yet in the IDE or usbconfig.h
//DIGITAL IO Cases
#define USB_MOD_FAN 0
#define USB_WRITE_CONF 1
#define USB_READ_CONF  2
#define USB_RESET_CONF  3
#define USB_READ_SENS  4

//Define EEPROM locations for restoring state
#define E_INIT 0
#define E_AUTO 1
#define E_PWM_01 2
#define E_PWM_02 3

static char replyBuf[16] = "Hello, USB!";
static uchar dataReceived = 0, dataLength = 0; // for USB_DATA_IN
static char adcVal[16] = "00000000aaaa0000";

int sw_portB = 0b00000001;
int sw_portC = 0b00000001;
int sw_portD = 0b00000001;
int sw_auto  = 0b00000111;
bool sw_led = true;
long tach_01 = 0;
long tach_02 = 0;
static long s = 0;

int lapse_01 = 0;
volatile int lapse_02 = 10;
volatile int countA = 0;
volatile int countB = 0;

volatile uint8_t portdhistory = 0xFF;

uint8_t pwm_01 = 0x00;
uint8_t pwm_02 = 0x00;

bool up_01 = true;
bool up_02 = true;


// this gets called when custom control message is received
USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *)data; // cast data to correct type
	
	switch(rq->bRequest) { // custom command is in the bRequest field
    case USB_READ_SENS: // send ADC data to PC
	    usbMsgPtr = adcVal;
        return sizeof(adcVal);
		return 0;
	case USB_WRITE_CONF: // Write actual PWM and autoPWM values to eeprom
		eeprom_write_byte((uint8_t*)E_AUTO,sw_auto);//Automatic pwm settings
		eeprom_write_byte((uint8_t*)E_PWM_01,pwm_01);//PWM 01 actual value
		eeprom_write_byte((uint8_t*)E_PWM_02,pwm_02);//PWM 02 actual value
	    usbMsgPtr = adcVal;
        return sizeof(adcVal);		
		return 0;	
	case USB_READ_CONF: // Read and restore PWM and autoPWM values from eeprom
		adcVal[8]=eeprom_read_byte((uint8_t*)E_AUTO);
		adcVal[1]=eeprom_read_byte((uint8_t*)E_PWM_01);		//Low byte		
		adcVal[3]=eeprom_read_byte((uint8_t*)E_PWM_02);		//Low byte		
	    usbMsgPtr = adcVal;
        return sizeof(adcVal);
		return 0;
	case USB_RESET_CONF: // Reset EEPROM values to initial
		eeprom_write_byte((uint8_t*)E_INIT,'T');//marks once that eeprom init is done
		eeprom_write_byte((uint8_t*)E_AUTO,0b00000111);//All autos active
		eeprom_write_byte((uint8_t*)E_PWM_01,0x80);//50%
		eeprom_write_byte((uint8_t*)E_PWM_02,0x80);//50%
		sw_auto=eeprom_read_byte((uint8_t*)E_AUTO);
		pwm_01=eeprom_read_byte((uint8_t*)E_PWM_01);
		pwm_02=eeprom_read_byte((uint8_t*)E_PWM_02);
		
		return 0;
	case USB_MOD_FAN: // modify automatic fan control and PWM values
		dataLength  = (uchar)rq->wLength.word;
        dataReceived = 0;
		
		if(dataLength  > sizeof(replyBuf)) // limit to buffer size
			dataLength  = sizeof(replyBuf);
		return USB_NO_MSG; // usbFunctionWrite will be called now
    }

    return 0; // should not get here
}

// This gets called when data is sent from PC to the device
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {
	uchar i;
			
	for(i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
		replyBuf[dataReceived] = data[i];
		
        uint8_t fourth = replyBuf[0] & 0xff ;
		adcVal[8] = fourth;
        sw_auto = fourth;
	if (sw_auto & 0b00000001)
		lapse_01 = 0;
	else
	{
        fourth = replyBuf[1] & 0xff ;
        pwm_01 = fourth;
	}	

	if (sw_auto & 0b00000010)
		lapse_01 = 0;
	else
	{
        fourth = replyBuf[2] & 0xff ;
        pwm_02 = fourth;
	}	
    return (dataReceived == dataLength); // 1 if we received it all, 0 if not
}

int main()
{
	if (eeprom_read_byte((uint8_t*)E_INIT)!='T')
	{
		eeprom_write_byte((uint8_t*)E_INIT,'T');//marks once that eeprom init is done
		eeprom_write_byte((uint8_t*)E_AUTO,0b00000111);//All autos active
		eeprom_write_byte((uint8_t*)E_PWM_01,0x80);//50%
		eeprom_write_byte((uint8_t*)E_PWM_02,0x80);//50%
		//once this procedure is held, no more initialization is performed
	}
	sw_auto=eeprom_read_byte((uint8_t*)E_AUTO);
	pwm_01=eeprom_read_byte((uint8_t*)E_PWM_01);
	pwm_02=eeprom_read_byte((uint8_t*)E_PWM_02);
	
	uchar i;
	uchar channel = 0;
	uchar channelcorrected = 0;


	DDRB = DDRB | 0b00001011; // PB0 PB1 PB3 as output Rest as input
	DDRD = DDRD | 0b01101000; // PD3 PD5 PD6 as output Rest as input

	DDRB   = DDRB & 0b11111011; // PB2 as input
	DDRD   = DDRD & 0b11101101; // PD1 PD4 as input
	
	PORTC = 0x7f; // turn On the Pull-ups
	PORTD |= (1 << PORTD1) | (1 << PORTD4); // turn On the Pull-up

	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt

	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(WGM22) | _BV(CS20);	

	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); //Phase correct
	TCCR2B = _BV(CS21);

	OCR2A = 255;
	OCR0A = 255;
	
	OCR2B = pwm_01;
	OCR0B = pwm_02;
		

    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();
	
    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++)
		{ // wait 500 ms
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    	}
    usbDeviceConnect();

	PCICR |= (1<<PCIE2);		// set PCIE2 to enable PCMSK2 scan
	PCMSK2 |= (1<<PCINT20);		// set PCINT20 to trigger an interrupt on state change
	PCMSK2 |= (1<<PCINT17);		// set PCINT17 to trigger an interrupt on state change

    sei(); // Enable interrupts after re-enumeration
//	OCR1B    = 6;  // 1 tic 0.000096 secs
	OCR1A    = 62;  // 1 Tic = 0.000992 Secs
//	OCR1B    = 625;  // 1 tic 0.01 secs

//	TCCR1B |= (1 << CS11);					// Start timer at Fcpu/8 16MHz/256=2.000.000 ticks per second.
//	TCCR1B |= (1 << CS11) | (1 << CS10);	// Start timer at Fcpu/64 16MHz/256=250.000 ticks per second.
	TCCR1B |= (1 << CS12);					// Start timer at Fcpu/256 16MHz/256=62500 ticks per second.
//	TCCR1B |= (1 << CS12) | (1 << CS10);	// Start timer at Fcpu/256 16MHz/1024=15625 ticks per second.
	
    while(1)
	{
        wdt_reset(); // keep the watchdog happy
        usbPoll();

		if (countB >= 100) //Have 100 clicks passed?
		{
			if (sw_auto & 0b00000001) //Is the auto PWM checked for fan 1 in the host?
			{
				pwm_01 += up_01 ? 1 : -1;
				if (pwm_01 == 255)
					up_01 = false;
				else if (pwm_01 == 0)
					up_01 = true;			
			}
	  		OCR0B = pwm_01;
			if (sw_auto & 0b00000010)  //Is the auto PWM checked for fan 2 in the host?
			{
				pwm_02 += up_02 ? 1 : -1;
				if (pwm_02 == 255)
					up_02 = false;
				else if (pwm_02 == 0)
					up_02 = true;		
			}
			OCR2B = pwm_02;
			countB = 0;
		}

		if(lapse_01 >= 100)
		{
				adcVal[0] =  0;		//High byte
				adcVal[1] = OCR0B;		//Low byte
	
				adcVal[2] =  0;		//High byte
				adcVal[3] = OCR2B;		//Low byte

				s = tach_01 * 302;			//Crude aproximation to RPM, 100 lapses are 0.992 secs.
				adcVal[4] =  s >> 8;		//High byte
				adcVal[5] = s & 0x00ff;		//Low byte
			
				s = tach_02 * 302;			//A DV07020_12U can run up to 6800 RPM according to it's datasheet.
				adcVal[6] =  s >> 8;		//High byte
				adcVal[7] = s & 0x00ff;		//Low byte

				adcVal[12] =  0;		//High byte
				adcVal[13] = lapse_01;		//Low byte

				adcVal[14] =  0;		//High byte
				adcVal[15] = channel;		//Low byte
				lapse_01 = 0;
				tach_01 = 0;
				tach_02 = 0;
		}
	}
    return 0;
}

ISR(TIMER1_COMPA_vect) //Timer 1 Compare A triggered.  We use Timer 1 for B port pins, no special reason for that.
{
countA++;
countB++;
lapse_01++;
if(sw_led) // Check which pins are activated and toogle. If not checked the led is is toggled one last time before stopping the blinking.
	{
	if (countA > 10000)
		{
		//PORTB = PORTB ^ 0b00000001; // Toggle  the  LED
		countA = 0;
		}
	}			
}
//Manage digital inputs
ISR(PCINT2_vect, ISR_NOBLOCK)
{
    uint8_t changedbits;

    changedbits = PIND ^ portdhistory;
    portdhistory = PIND;
	
	if(changedbits & (1 << PIND1))
	{
		PORTB = PORTB ^ 0b00000010; // Toggle a LED
		tach_01++;
	}
			
	if(changedbits & (1 << PIND4))
	{
		PORTB = PORTB ^ 0b00000001; // Toggle a LED
		tach_02++;
		
	}
}

