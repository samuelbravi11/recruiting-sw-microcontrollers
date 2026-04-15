#ifndef GPIO_ADC_H
#define GPIO_ADC_H

#include <stdbool.h>

#define GPIO_PIN_ADC GPIO_PIN_1
#define GPIO_PORT_ADC GPIOA

void GPIO_start_receive(void);
void GPIO_stop_receive(void);
bool GPIO_is_receiving(void);

#endif