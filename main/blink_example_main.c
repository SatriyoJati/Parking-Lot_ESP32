/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "FreeAct.h"
#include "keypad.h"


Keypad mykeypad =
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},

        {5, 18, 19, 21},
        {12, 14, 27, 26},

        .rowLen = 4,
        .columnLen = 4,

        .outChar = 'A',
};

void app_main(void)
{
    Keypad_init(&mykeypad);
    char buf;
    // vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreate(vKeypadTask, "keypad_task", 2000, (void *) &mykeypad, 1,NULL);
    
}
