#include "fsm.h"
#include "cli.h"
#include "uart.h"
#include "timers.h"
#include "process_state.h"
#include "main.h"
#include "dma_adc.h"
#include "dma_controller.h"
#include "queue_event.h"
#include <string.h>

extern ADC_HandleTypeDef hadc1;

enum fsm_state curr_state = STATE_INIT;
enum fsm_state prev_state = STATE_ERROR; // set to error to force process_state at the first dispatch --> the only to go in init is from error

void (* const fsm_process_state[NUM_STATES])(void) = {
  process_init,         // init
  process_wait_request, // wait request
  process_listening,    // listening
  process_pause,        // pause
  process_warning,      // warning
  process_error         // error
};


static void fsm_init(void) {
   curr_state = STATE_WAIT_REQUEST;
}

static void fsm_wait_request(void) {
   enum CliCommand cmd;
   if (cli_run(&cmd) == FSM_ERROR) {
      enqueue_event(EVENT_INVALID);
   } else {
      set_process_callback();
   }
}

static void fsm_listening(void) {
   check_and_process();
}

static void fsm_pause(void) {
   enum CliCommand cmd;
   if (cli_run(&cmd) == FSM_ERROR) {
      enqueue_event(EVENT_INVALID);
   } else {
      set_process_callback();
   }
}

static void fsm_warning(void) {
   char *msg_warn = "WARNING\r\n";
   if (UART_transmit(msg_warn, strlen(msg_warn)) == FSM_ERROR) {
      enqueue_event(EVENT_INVALID);
   }
   // HAL_Delay(500); // TODO togli
}

static void fsm_error(void) {
   char *msg_err = "ERROR\r\n";
   if (UART_transmit(msg_err, strlen(msg_err)) == FSM_ERROR) {
      enqueue_event(EVENT_INVALID);
   }
   // HAL_Delay(500); // TODO togli
}

// TODO INVALID --> error



static void go_init(void) {
   curr_state = STATE_WAIT_REQUEST;
}

static void go_wait_request(void) {
   curr_state = STATE_WAIT_REQUEST;
}

static void go_listen(void) {
   curr_state = STATE_LISTENING;
}

static void go_pause(void) {
   curr_state = STATE_PAUSE;
}

static void go_warning(void) {
   curr_state = STATE_WARNING;
}

static void go_error(void) {
   curr_state = STATE_ERROR;
}


void (* const fsm_actions_state[NUM_STATES])(void) = {
  fsm_init,         // init
  fsm_wait_request, // wait request
  fsm_listening,    // listening
  fsm_pause,        // pause
  fsm_warning,      // warning
  fsm_error         // error
};


// fsm is a matrix state*actions of a pointer to const functions that takes void ad parameters and return void
static void (* const fsm_table[NUM_STATES][NUM_CMD])(void) = {
  /* states:    cli_cmd                , tim_exp       , btn_prssd        , invalid    */
  /* init    */ {NULL                  , NULL          , NULL             , go_error   },
  /* w_rqst */  {set_process_callback  , go_error      , go_listen        , go_error   },
  /* lstn  */   {go_error              , go_warning    , go_pause         , go_error   },
  /* pause */   {set_process_callback  , go_error      , go_listen        , go_error   },
  /* warning */ {NULL                  , NULL          , go_wait_request  , go_error   },
  /* error */   {NULL                  , NULL          , go_init          , NULL       },
};


void fsm_dispatch() {
   // 1) takes the event from an array
   enum fsm_event event = dequeue_event();

   // 2) if event != none do fsm_table fun
   if (event != EVENT_NONE) {
      fsm_table[curr_state][event]();
   }

   // 3) check if curr_state != prev_state and in case do process_state
   if (curr_state != prev_state) {
      prev_state = curr_state;
      fsm_process_state[curr_state]();
   }

   // 4) do action based on state 
   fsm_process_state[curr_state]();
}