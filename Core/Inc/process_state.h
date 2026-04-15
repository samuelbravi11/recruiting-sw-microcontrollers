#ifndef PROCESS_FUNC_H
#define PROCESS_FUNC_H

#include "fsm_types.h"

FSM_RET process_init(void);
FSM_RET process_wait_request(void);
FSM_RET process_listening(void);
FSM_RET process_pause(void);
FSM_RET process_warning(void);
FSM_RET process_error(void);

#endif