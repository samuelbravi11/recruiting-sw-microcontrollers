#ifndef ACTIONS
#define ACTIONS

#include "cli.h"
#include "fsm_types.h"
#include "main.h"
#include <stdint.h>

#define DIM_MSG_ERR 64
#define NUM_STATES 6

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

void set_error_message(const char *msg);
void fsm_dispatch();

#endif