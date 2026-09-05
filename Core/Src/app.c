//
// Created by arnav004 on 9/5/26.
//
#include "app.h"

static volatile uint8_t g_tick_flag = 0;
static uint32_t g_tick_count = 0;
static uint32_t g_target_ticks = 10;

void App_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        g_tick_flag = 1;
    }
}

void App_Run(void)
{
    if (!g_tick_flag) return;
    g_tick_flag = 0;

    g_tick_count++;
    if (g_tick_count >= g_target_ticks) {
        g_tick_count = 0;
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
}