#ifndef QUEUE_EVENT_H
#define QUEUE_EVENT_H

#include <stdbool.h>

#define QUEUE_SIZE 20

#define NUM_EVENTS 5

enum fsm_event {
   EVENT_NONE,
   EVENT_CLI_COMMAND, // enum CliCommand
   EVENT_TIMER_EXPIRED,
   EVENT_BUTTON_PRESSED,
   EVENT_INVALID
};

int queue_full(void);
int queue_empty(void);
void queue_clean(void);
bool enqueue_event(enum fsm_event event);
enum fsm_event dequeue_event(void);

#endif