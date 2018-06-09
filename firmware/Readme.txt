This is the Readme file for VUSiBino Demo and related code.
/**
 * Project: VUSiBino demo https://chafalladas.com/vusibino-2
 * Author: Alfonso Abelenda Escudero alfonso@abelenda.es
 * Inspired by V-USB example code by Christian Starkjohann and v-usb tutorials from http://codeandlife.com
 * Hardware based on tinyUSBboard http://matrixstorm.com/avr/tinyusbboard/ and paperduino perfboard from http://txapuzas.blogspot.com.es
 * Copyright: (C) 2017 Alfonso Abelenda Escudero
 * License: GNU GPL v3 (see License.txt)
 */

WHAT IS INCLUDED IN THIS PACKAGE?
=================================
This package consists of the device side USB driver firmware, and host code:

  Readme.txt .............. The file you are currently reading.
  usbdrv .................. V-USB firmware.
  Host .................... Code for device at host side.
  Firmware ................ Code for device.
  Schematics .............. Diagrams for the circuit

OBJECTIVE.
==========

Demonstrate how to comunicate with an AVR using v-usb. Change the state of a digital port on the fly, send and receive
text from the host to the device, and change dinamically the serial number of the device.
