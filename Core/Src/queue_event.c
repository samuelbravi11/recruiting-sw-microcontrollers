#include "queue_event.h"

enum fsm_event event_queue[QUEUE_SIZE];
int head = 0;

int queue_empty(void) {
   return head == 0;
}

void enqueue_event(enum fsm_event event) {
   event_queue[head] = event;
   head = (head + 1) % QUEUE_SIZE;
}

enum fsm_event dequeue_event(void) {
  if (queue_empty()) {
    return EVENT_NONE; // or some error code
  }
  head -= 1;
  enum fsm_event event = event_queue[head];
  return event;
}