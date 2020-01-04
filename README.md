# BLE MIDI Server Service Demo

## Scope

This demo shows how to run a BLE MIDI Service

You should be able to connect to the Bluetooth MIDI Interface with your computer.

E.g. MacOS: in Audio/MIDI Config, MIDI Window, press Bluetooth Configuration button: the "MIDIbox" device should show up, and you should be able to connect.

The demo will send Note On/Off messages each second, in addition, incoming MIDI messages will be loopbacked


Re-usable component is located under components/blemidi - please see the README.md for programmers there.


## Important

Please optimize the app configuration with "idf.py menuconfig":

* Compiler Options->Optimization Level: set to -Os (Release)
* Component Config->ESP32 Specific: set Minimum Supported ESP32 Revision to 1 (if you have a newer device...)
* Component Config->ESP32 Specific: set CPU frequency to 240 MHz
