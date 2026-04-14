#include "cli.h"
#include "fsm.h"
#include "uart.h"
#include "queue_event.h"
#include <stdbool.h>
#include <string.h>

bool volatile cli_ready = false;
enum CliCommand volatile cli_last_cmd = CMD_NONE;

// CLI in responsable of reading user input and parsing it to a command that can be used by the FSM. It also used to write on screen what user send via USART.

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

static void cli_print(const char *msg) {
   UART_transmit(msg, strlen(msg));
   UART_transmit("\r\n", 2);
}

FSM_RET cli_run(enum CliCommand *cmd) {
   if (!UART_is_running() || (UART_poll_errors() == FSM_ERROR)) {
      return FSM_ERROR;
   }
   char *cmd_str = UART_get_user_input();
   if (cmd_str) {
      cli_print(cmd_str);
      *cmd = cli_parse(cmd_str);
      // if cmd is valid enqueue event to process it in the fsm dispatch
      if (*cmd != CMD_INVALID) {
         cli_last_cmd = *cmd;
         enqueue_event(EVENT_CLI_COMMAND);
      }
   }
   return FSM_OK;
}