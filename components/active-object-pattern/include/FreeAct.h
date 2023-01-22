#include "freertos/queue.h"

enum ReversedSignal{
    INIT_SG,
    USER_SG
};

typedef struct {
    uint16_t sig;
    //extensible 
}Event;

typedef struct Active Active;

typedef void (*DispatchHandler) (Active * const me, Event const* const e);

struct Active{
    TaskHandle_t thread;
    StaticTask_t thread_cb;

    QueueHandle_t queue;
    StaticQueue_t queue_cb;

    DispatchHandler dispacth;
};


void Active_ctor(Active * const me, DispatchHandler dispatch);
void Active_start(Active * const me, 
                uint8_t prio, 
                Event **queueSto,
                uint32_t queuelen,
                StackType_t *stackSto,
                uint32_t stackDepth);

void Active_post(Active* const me, Event const* const e){
}