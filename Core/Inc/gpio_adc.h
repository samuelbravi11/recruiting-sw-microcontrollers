#ifndef GPIO_ADC_H
#define GPIO_ADC_H

#include <stdbool.h>

#define GPIO_PIN_ADC 0x0002
#define GPIO_PORT_ADC ((0x50000000UL) + 0x00000000UL)

void GPIO_start_receive(void);
void GPIO_stop_receive(void);
bool GPIO_is_receiving(void);

#endif