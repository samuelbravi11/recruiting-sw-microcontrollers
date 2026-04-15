#include "queue_event.h"
#include "fsm.h"
#include "main.h"
#include <stdbool.h>

enum fsm_event event_queue[QUEUE_SIZE];
static volatile int head = 0;
static volatile int tail = 0;

int queue_full(void) { return ((head + 1) % QUEUE_SIZE) == tail; }

int queue_empty(void) { return head == tail; }

void queue_clean(void) {
   head = 0;
   tail = 0;
}

bool enqueue_event(enum fsm_event event) {
   __disable_irq();
   if (queue_full()) {
      __enable_irq();
      set_error_message("Event queue full");
      return false;
   }
   event_queue[head] = event;
   head = (head + 1) % QUEUE_SIZE;
   __enable_irq();
   return true;
}

enum fsm_event dequeue_event(void) {
   __disable_irq();
   if (queue_empty()) {
      __enable_irq();
      return EVENT_NONE;
   }
   enum fsm_event event = event_queue[tail];
   tail = (tail + 1) % QUEUE_SIZE;
   __enable_irq();
   return event;
}