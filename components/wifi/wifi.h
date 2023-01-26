
#ifndef WIFI_H
#define WIFI_H


//hanya handle
typedef struct Wifi* Wifi_handle_t;


//create instance
Wifi_handle_t Wifi_create(void);

//free memory instance
void Wifi_destroy(Wifi_handle_t me);

//Setup wifi
void Wifi_init(Wifi_handle_t me);


#endif