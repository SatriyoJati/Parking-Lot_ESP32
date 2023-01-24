#include "keypad.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define COUNT 8
#define DEBOUNCING

static const char matrix_keys[4][4] = {{'1', '2', '3', 'A'},
                                {'4', '5', '6', 'B'},
                                {'7', '8', '9', 'C'},
                                {'*', '+', '#', 'D'}};

static void initialize_pins(Keypad *pkeypad)
{
    int i;

    for (i = 0; i < pkeypad->columnLen; i++)
    {
        gpio_set_direction(pkeypad->columnPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(pkeypad->columnPins[i], 1);
    }

    for (i = 0; i < pkeypad->rowLen; i++)
    {
        gpio_set_direction(pkeypad->rowPins[i], GPIO_MODE_INPUT);
        gpio_pullup_en(pkeypad->rowPins[i]);
        gpio_set_pull_mode(pkeypad->rowPins[i], GPIO_PULLUP_ONLY);
    }
}

static int Keypad_debounce(gpio_num_t rowPin)
{
    static uint32_t count = COUNT;
    static uint8_t debounced_state = false;
    uint8_t key_pressed = false;
    uint8_t raw_state = gpio_get_level(rowPin);
    if (raw_state == debounced_state)
    {
        printf("Key_Debounce : state is same\n");
        if (debounced_state)
            count = COUNT;
        else
            count = COUNT;
    }
    else
    {
        printf("Key_Debounce : Change during debouncing\n");
        printf("Key_Debounce : count : %ld\n", count);
        if (--count == 0)
        {
            // printf("Key_Debounce : Finally Stable ------whooozaaah\n");
            debounced_state = raw_state;
            key_pressed = true;
            if (debounced_state)
                count = COUNT;
            else
                count = COUNT;
            return 0;
        }
    }
    return -1;
}

void Keypad_init(Keypad *pkeypad)
{
    uint8_t rowLen = sizeof(pkeypad->row) / sizeof(uint8_t);
    uint8_t columnLen = sizeof(pkeypad->column) / sizeof(uint8_t);

    // initialize pins
    initialize_pins(pkeypad);
    printf("Keypad pins initialized\n");
}

char Keypad_scan(Keypad *pkeypad)
{
    // set first column low
    // scan all row which one should LOW then it is the row, save the value row_cliked
    // save the value column clicked
    // if not found set next column low , repeat

    int i, j;
    i = j = 0;

    uint8_t row_get = 0;
    uint8_t column_get = 0;

    for (j = 0; j < pkeypad->columnLen; j++)
    {
        gpio_set_level(pkeypad->columnPins[j], 1);
    }

    for (j = 0; j < pkeypad->columnLen; j++)
    {
        // printf("column : %d\n", j);
        gpio_set_level(pkeypad->columnPins[j], 0);
        for (i = 0; i < pkeypad->rowLen; i++)
        {
            int buf = 0;
            int debounce = 0;
            // TODO add debounce on gpio_get_level before assign to buf.
            buf = Keypad_debounce(pkeypad->rowPins[i]);
            // printf("row at %d col at %d is %d\n", i,j, buf);
            if (buf == 0)
            {
                if (buf == 0)
                {
                // #ifdef DEBOUNCING
                    #ifdef DEBOUNCING
                    debounce = Keypad_debounce(pkeypad->rowPins[i]);
                    printf("Keypad_scan : debounce output : %d\n", debounce);
                    if (debounce == 0)
                    {
                    #endif
                        row_get = i;
                        column_get = j;
                        printf("Press coordinate : (%d,%d)", i, j);
                        return matrix_keys[i][j];
                    #ifdef DEBOUNCING
                    }
                    #endif
                }
            }
            gpio_set_level(pkeypad->columnPins[j], 1);
            // printf("\n");
        }
        
    }
    return 0;
}

void vKeypadTask(void *pvParameters)
{
    char keypressed;

    Keypad *pkeypad = (Keypad *)pvParameters;
    QueueHandle_t xKeyQueue = xQueueCreate(10, sizeof(Keypad));
    BaseType_t status_send = pdFAIL;
    xQueueReset(xKeyQueue);
    printf("number of messages in queue: %d", uxQueueMessagesWaiting(xKeyQueue));

    for (;;)
    {
        // printf("Send data error");
        keypressed = Keypad_scan(pkeypad);
        // printf("KeyGet : %c\n",keypressed);
        if (keypressed != 0)
        {
            status_send = xQueueSendToFront(xKeyQueue, &keypressed, 0);
            if (status_send == pdPASS)
            {
                printf("\nReceived %c\n", keypressed);
            }
            else
            {
                printf("queue is full\n\n");
            }
        }

        // vTaskDelay(pdMS_TO_TICKS(100));
    }
    vQueueDelete(xKeyQueue);
    vTaskDelete(NULL);
}
