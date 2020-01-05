# BLE MIDI Server

## Introduction

This BLE server is based on the "ESP-IDF Gatt Server Service Table Demo" located under 
$IDF_PATH/examples/bluetooth/gatt_server_service_table

Additional sources (very helpful for somebody new to Bluetooth):

* https://www.novelbits.io/bluetooth-gatt-services-characteristics/
* https://devzone.nordicsemi.com/nordic/nordic-blog/b/blog/posts/midi-over-bluetooth-le
* https://talk.vanhack.ca/t/ble-midi-kit-dev-blog/8241

And especially the very easy to use Arduino implementation, which I used as a reference during bringup:
* https://github.com/neilbags/arduino-esp32-BLE-MIDI/blob/master/BLE_MIDI.ino


## Scope

This solution is not claimed to be the better one compared to the one provided to the Arduino platform.

I just wanted to bring it up from scratch by myself, and may need more control over the APIs in future
based on my knowledge with MIOS32 (-> http://www.uCApps.de)


## Usage

See also the demo application under ../../main/blemidi_demo.c


### Initialization

Launch the BLE Server with:

```c
void app_main()
{
  // install BLE MIDI service
  int status = blemidi_init(callback_midi_message_received);
  if( status < 0 ) {
    ESP_LOGE(BLEMIDI_TAG, "BLE MIDI Driver returned status=%d", status);
  } else {
    ESP_LOGI(BLEMIDI_TAG, "BLE MIDI Driver initialized successfully");
    xTaskCreate(task_midi, "task_midi", 4096, NULL, 8, NULL);    
  }
}
```

and:
* register a callback for incoming MIDI messages
* launch a task for sending MIDI messages


### Receiving MIDI

See callback_midi_message_received() function in blemidi_demo.c.

API:
```c
void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len)
```

* blemidi_port is currently always 0!
* timestamp is based on BLE MIDI spec (1 mS resolution)
* midi_status contains the MIDI Status byte - running status has already been considered by the driver
* remaining_message and len: the remaining bytes (typically 2, e.g. Note or CCs, or much more on SysEx streams)


### Sending MIDI

See task_midi() function in blemidi_demo.c.

Currently no comfortable API exists, it might come later:
```c
    {
      // TODO: more comfortable packet creation via special APIs
      uint8_t message[3] = { 0x90, 0x3c, 0x7f };
      blemidi_send_packet(0, message, sizeof(message));
    }
```
