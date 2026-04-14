#ifndef CLI_H
#define CLI_H

#include "fsm_types.h"
#include <stdbool.h>

#define NUM_CMD 5

enum CliCommand {
   CMD_NONE,
   CMD_RAW,
   CMD_MOVING_AVERAGE,
   CMD_RANDOM_NOISE,
   CMD_INVALID
};

extern enum CliCommand volatile cli_last_cmd;

FSM_RET cli_run(enum CliCommand *cmd);

#endif