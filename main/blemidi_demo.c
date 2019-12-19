/*
 * BLE MIDI Demo
 *
 * See ../README.md for usage hints
 *
 * =============================================================================
 *
 * MIT License
 *
 * Copyright (c) 2019 Thorsten Klose (tk@midibox.org)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * =============================================================================
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "blemidi.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// This task is periodically called to send a MIDI message
////////////////////////////////////////////////////////////////////////////////////////////////////
static void task_midi(void *pvParameters)
{
  portTickType xLastExecutionTime;
  unsigned ctr = 0;

  // Initialise the xLastExecutionTime variable on task entry
  xLastExecutionTime = xTaskGetTickCount();

  while( 1 ) {
    vTaskDelayUntil(&xLastExecutionTime, 500 / portTICK_RATE_MS);

    blemidi_tick_ms(500); // for timestamp handling

    ctr += 1;
    ESP_LOGI(BLEMIDI_TAG, "Sending MIDI Note #%d", ctr);

    {
      // TODO: more comfortable packet creation via special APIs
      uint8_t packet[5] = { blemidi_timestamp_high(), blemidi_timestamp_low(), 0x90, 0x3c, 0x7f };
      blemidi_send_packet(0, packet, 5);
    }
    
    vTaskDelayUntil(&xLastExecutionTime, 500 / portTICK_RATE_MS);

    blemidi_tick_ms(500); // for timestamp handling

    {
      // TODO: more comfortable packet creation via special APIs
      uint8_t packet[5] = { blemidi_timestamp_high(), blemidi_timestamp_low(), 0x90, 0x3c, 0x00 };
      blemidi_send_packet(0, packet, 5);
    }

  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// This callback is called whenever a new MIDI message is received
////////////////////////////////////////////////////////////////////////////////////////////////////
void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos)
{
  ESP_LOGI(BLEMIDI_TAG, "CALLBACK blemidi_port=%d, timestamp=%d, midi_status=0x%02x, len=%d, continued_sysex_pos=%d, remaining_message:", blemidi_port, timestamp, midi_status, len, continued_sysex_pos);
  esp_log_buffer_hex(BLEMIDI_TAG, remaining_message, len);

  // loopback received message
  {
    // TODO: more comfortable packet creation via special APIs

    // Note: by intention we create new packets for each incoming message
    // this shows that running status is maintained, and that SysEx streams work as well
    
    size_t loopback_packet_len = 3 + len; // includes timestamp, MIDI status and remaining bytes
    uint8_t *loopback_packet = (uint8_t *)malloc(loopback_packet_len * sizeof(uint8_t));
    if( loopback_packet == NULL ) {
      // no memory...
    } else {
      loopback_packet[0] = blemidi_timestamp_high();
      loopback_packet[1] = blemidi_timestamp_low();
      loopback_packet[2] = midi_status;
      memcpy(&loopback_packet[3], remaining_message, len);

      blemidi_send_packet(0, loopback_packet, loopback_packet_len);

      free(loopback_packet);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// Application Starting Point
////////////////////////////////////////////////////////////////////////////////////////////////////
void app_main()
{
  // install BLE MIDI service
  int status = blemidi_init(callback_midi_message_received);
  if( status < 0 ) {
    ESP_LOGE(BLEMIDI_TAG, "BLE MIDI Driver returned status=%d", status);
  } else {
    ESP_LOGI(BLEMIDI_TAG, "BLE MIDI Driver initialized successfully");
    xTaskCreate(task_midi, "task_midi", 2048, NULL, 8, NULL);    
  }
}
