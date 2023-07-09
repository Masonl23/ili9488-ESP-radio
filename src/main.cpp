#include <Arduino.h>
#include <WiFi.h>
#include "GUIClass.h"

// for disabling watchdog timer on dual core processes...
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// // For using both cores of the esp32, this is mitigate lag in the USER interface
TaskHandle_t TFThelper;  // TFT helper assigns one core to only deal with graphics
TaskHandle_t WifiHelper; // WiFiHelper assigns another core to only deal with WiFi commands

void GUIloop(void *param);
void WiFiloop(void *param);

GUIClass gui;

void setup()
{
  Serial.begin(115200);
  vTaskDelay(1);
  xTaskCreatePinnedToCore(GUIloop, "GUIHelper", 16000, NULL, 1, &TFThelper, 1);
  delay(500);
  xTaskCreatePinnedToCore(WiFiloop, "WiFiHelper", 16000, NULL, 0, &WifiHelper, 0);

  gui.Start();
}

void loop()
{
  vTaskDelete(NULL);
  delay(1);
}

void GUIloop(void *param){
  delay(500);
  // TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;       // prevent watchdog from crashing program!
  // TIMERG0.wdt_feed = 1;
  // TIMERG0.wdt_wprotect = 0;
    while(true){
      gui.CheckButtonPress();

      gui.RefreshData();
      vTaskDelay(1);
    }
}

void WiFiloop(void *param){

  // TIMERG1.wdtwprotect = TIMG_WDT_WKEY_VALUE;       // prevent watchdog from crashing program!
  // TIMERG1.wdtfeed = 1;
  // TIMERG1.wdtwprotect = 0;
  

    while(true){
      gui.CheckQueue();
      vTaskDelay(1);
    }
}


