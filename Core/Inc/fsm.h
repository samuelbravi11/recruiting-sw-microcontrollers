#ifndef ACTIONS
#define ACTIONS

#include "cli.h"
#include "fsm_types.h"
#include "main.h"
#include <stdint.h>

#define NUM_STATES 6
#define NUM_EVENTS 5

// TODO --> make private
enum fsm_state {
  STATE_INIT,
  STATE_WAIT_REQUEST,
  STATE_LISTENING,
  STATE_PAUSE,
  STATE_WARNING,
  STATE_ERROR
};

extern enum fsm_state curr_state;
extern enum fsm_state prev_state;

// array of pre-functions, used before first time entering in a state
extern void (* const fsm_pre_func[NUM_STATES])(void);

void fsm_dispatch();

#endif