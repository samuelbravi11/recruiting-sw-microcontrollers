#ifndef QUEUE_EVENT_H
#define QUEUE_EVENT_H

#define QUEUE_SIZE 10

enum fsm_event {
  EVENT_NONE,
  EVENT_BUTTON_PRESSED,
  EVENT_TIMER_EXPIRED,
  EVENT_CLI_COMMAND, // enum CliCommand
  EVENT_INVALID
};

int queue_empty(void);
void enqueue_event(enum fsm_event event);
enum fsm_event dequeue_event(void);

#endif