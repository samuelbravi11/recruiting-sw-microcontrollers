#ifndef PROCESS_FUNC_H
#define PROCESS_FUNC_H

void process_init(void);
void process_wait_request(void);
void process_listening(void);
void process_pause(void);
void process_warning(void);
void process_error(void);

#endif