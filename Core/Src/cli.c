#include "cli.h"
#include "dma_usart_tx.h"
#include "fsm.h"
#include "fsm_types.h"
#include "queue_event.h"
#include "uart.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool volatile cli_ready = false;
enum CliCommand volatile cli_last_cmd = CMD_NONE;
char frmt_msg[DIM_BUF_INFO];

// CLI in responsable of reading user input and parsing it to a command that can
// be used by the FSM. It also used to write on screen what user send via USART.

static enum CliCommand cli_parse(const char *buff) {
   if (!strcmp(buff, "raw")) {
      return CMD_RAW;
   } else if (!strcmp(buff, "moving average")) {
      return CMD_MOVING_AVERAGE;
   } else if (!strcmp(buff, "random noise")) {
      return CMD_RANDOM_NOISE;
   } else {
      return CMD_INVALID;
   }
}

static FSM_RET cli_print_cmd_valid(const char *msg) {
   snprintf(frmt_msg, sizeof(frmt_msg), "callback attivata: %s\r\n", msg);
   if (!DMA_TX_transfer_buf((uint8_t *)frmt_msg, strlen(frmt_msg)))
      return FSM_BUSY;
   return FSM_OK;
}

// top 3 func
FSM_RET cli_run(enum CliCommand *cmd) {
   if (UART_poll_errors() == FSM_ERROR) {
      return FSM_ERROR;
   }
   char *cmd_str = UART_get_user_input();
   if (cmd_str) {
      *cmd = cli_parse(cmd_str);
      if (*cmd != CMD_INVALID) {
         cli_print_cmd_valid(cmd_str);
         cli_last_cmd = *cmd;
         // if cmd is a CLI cmd enqueue event cli
         if (!enqueue_event(EVENT_CLI_COMMAND))
            return FSM_ERROR;
      }
      return FSM_OK; // user input processed
   }
   if (!UART_is_running() && UART_start_receive() != FSM_OK) {
      set_error_message("UART was not running and restart failed");
      return FSM_ERROR;
   }
   return FSM_BUSY; // user input not ready
}