//
// Created by arnav004 on 9/5/26.
//

#ifndef UART_BLINK_RATE_APP_H
#define UART_BLINK_RATE_APP_H

#include "main.h"
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;

void App_Init(void);
void App_Run(void);

#endif //UART_BLINK_RATE_APP_H
