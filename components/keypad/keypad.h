#include <stdint.h>


#ifndef KEYPAD_H
#define KEYPAD_H

typedef struct
{
    uint8_t row[4];
    uint8_t column[4];

    uint8_t rowPins[4];
    uint8_t columnPins[4];
    
    uint8_t rowLen;
    uint8_t columnLen;

    uint8_t outChar;
} Keypad;


void Keypad_init(Keypad *pkeypad);

#ifdef ORIGINAL_KEYPAD
char Keypad_scan(Keypad* keypad);
#endif 
char Keypad_scan(Keypad* keypad);
void Keypad_debounce();


void vKeypadTask(void * pvParameters);
#endif