#include "keypad.h"
#include "driver/gpio.h"

const char matrix_keys[4][4] = {{'1', '2', '3', 'A'},
                                {'4', '5', '6', 'B'},
                                {'7', '8', '9', 'C'},
                                {'*', '+', '#', 'D'}};

static void initialize_pins(Keypad * pkeypad)
{
    int i;

    for(i=0;i<pkeypad->columnLen; i++)
    {
        gpio_set_direction(pkeypad->columnPins[i], GPIO_MODE_OUTPUT);
    }

    for(i=0;i<pkeypad->rowLen;i++)
    {
        gpio_set_direction(pkeypad->rowPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(pkeypad->rowPins[i], GPIO_PULLUP_ONLY);
    }

}

void Keypad_init(Keypad *pkeypad)
{
    uint8_t rowLen = sizeof(pkeypad->row)/sizeof(uint8_t);
    uint8_t columnLen = sizeof(pkeypad->column) / sizeof(uint8_t);

    //initialize pins
    initialize_pins(pkeypad);
    printf("Keypad pins initialized\n");
}

char Keypad_scan(Keypad *pkeypad)
{
    //set first column low
    //scan all row which one should LOW then it is the row, save the value row_cliked
    //save the value column clicked
    //if not found set next column low , repeat

    int i,j;
    i=j=0;

    uint8_t row_get=0;
    uint8_t column_get=0;
    for(j=0;j<pkeypad->columnLen;j++)
    {
        printf("column : %d\n", j);
        gpio_set_level(pkeypad->columnPins[j], 0);
        for(i=0;i<pkeypad->rowLen;i++)
        {
            int buf=0;
            //TODO add debounce on gpio_get_level before assign to buf.
            buf = gpio_get_level(pkeypad->rowPins[i]);
            printf("scan rows %d", i);
            if(buf == 0)
            {
                row_get=i;
                column_get = j;
                return matrix_keys[i][j];
            }
        }
        printf("\n");
    }
    return -1;
}

void vKeypadTask( void *pvParameters)
{
    char keypressed;
    
    Keypad Struct = (Keypad) *pvParameters;
    QueueHandle_t xKeyQueue = xQueueCreate(8, sizeof(Keypad));
    BaseType_t status_send;

    for(;;)
    {
        keypressed = Keypad_scan();
        if (keypressed != -1)
            status_send = xQueueSendToFront(xKeyQueue, &keypressed, pdMS_TO_TICKS(10));

        if(status_send == pdPASS)
        {
            printf("Received %c", keypressed);
        }
        else
        {
            printf("queue is full");
        }
    }

    vTaskDelete( NULL);
}
