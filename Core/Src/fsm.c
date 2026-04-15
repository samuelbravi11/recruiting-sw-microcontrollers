#include "fsm.h"
#include "cli.h"
#include "uart.h"
#include "timers.h"
#include "process_state.h"
#include "main.h"
#include "dma_adc.h"
#include "dma_usart_tx.h"
#include "dma_controller.h"
#include "queue_event.h"
#include <string.h>
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

enum fsm_state curr_state = STATE_INIT;
enum fsm_state prev_state = STATE_ERROR; // set to error to force process_state at the first dispatch --> the only to go in init is from error

char fsm_error_msg[DIM_MSG_ERR] = "ERROR: Unknown error\r\n";

FSM_RET (* const fsm_process_state[NUM_STATES])(void) = {
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
      queue_clean();
      enqueue_event(EVENT_INVALID);
   }
}

static void fsm_listening(void) {
   if (check_and_process() == FSM_ERROR) {
      queue_clean();
      enqueue_event(EVENT_INVALID);
   }
}

static void fsm_pause(void) {
   enum CliCommand cmd;
   if (cli_run(&cmd) == FSM_ERROR) {
      queue_clean();
      enqueue_event(EVENT_INVALID);
   }
}

static void fsm_warning(void) {
   static uint32_t last = 0;
   const char *msg_warn = "WARNING\r\n";
   if (HAL_GetTick() - last >= 200) {
      last = HAL_GetTick();
      DMA_TX_transfer_buf((uint8_t *)msg_warn, strlen(msg_warn));
   }
}

static void fsm_error(void) {
   static uint32_t last = 0;
   // spam every 200 ms the error message
   if (HAL_GetTick() - last >= 200) {
      last = HAL_GetTick();
      DMA_TX_transfer_buf((uint8_t *)fsm_error_msg, strlen(fsm_error_msg));
   }
}


// TODO INVALID --> error
void set_error_message(const char *msg) {
   // "\r\n\0" --> 3 characters
   if (strlen(msg) >= DIM_MSG_ERR - 3) {
      snprintf(fsm_error_msg, DIM_MSG_ERR, "Unknown error, message too long\r\n");
   } else {
      snprintf(fsm_error_msg, DIM_MSG_ERR, "ERROR: %s\r\n", msg);
   }
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

static void reset_mcu(void) {
   NVIC_SystemReset();
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
static void (* const fsm_table[NUM_STATES][NUM_EVENTS])(void) = {
  /* states:    cli_cmd                , tim_exp       , btn_prssd        , invalid    */
  /* init    */ {NULL                  , NULL          , NULL             , go_error   },
  /* w_rqst */  {set_process_callback  , go_error      , go_listen        , go_error   },
  /* lstn  */   {go_error              , go_warning    , go_pause         , go_error   },
  /* pause */   {set_process_callback  , go_error      , go_listen        , go_error   },
  /* warning */ {NULL                  , NULL          , go_wait_request  , go_error   },
  /* error */   {NULL                  , NULL          , reset_mcu        , NULL       },
};


void fsm_dispatch() {
   // 1) takes the event from an array
   enum fsm_event event = dequeue_event();
   // 2) if event != none do fsm_table fun
   if (event != EVENT_NONE && fsm_table[curr_state][event-1]) {
      fsm_table[curr_state][event-1]();   // -1 because of the EVENT_NONE = 0
   }
   // 3) check if curr_state != prev_state and in case do process_state
   if (curr_state != prev_state) {
      prev_state = curr_state;
      if (fsm_process_state[curr_state]() == FSM_ERROR) {
         go_error();
      }
   }
   // 4) do action based on state 
   if (fsm_actions_state[curr_state] != NULL) {
      fsm_actions_state[curr_state]();
   }
}