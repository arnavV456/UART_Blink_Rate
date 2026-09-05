#include "app.h"
#include <stdio.h>
#include <string.h>

#define CMD_BUF_SIZE   32
#define MIN_BLINK_MS   150
#define MAX_BLINK_MS   5000
#define TICK_MS        10

static volatile uint8_t g_tick_flag = 0;
static uint32_t g_tick_count = 0;
static uint32_t g_target_ticks = 50;   // 500ms default

static uint8_t g_rx_byte;
static char g_cmd_buf[CMD_BUF_SIZE];
static uint8_t g_cmd_index = 0;

static void Process_Command(char *cmd)
{
    char keyword[16];
    int value;

    if (sscanf(cmd, "%15s %d", keyword, &value) == 2) {
        if (strcmp(keyword, "SET_RATE") == 0) {
            if (value < MIN_BLINK_MS) value = MIN_BLINK_MS;
            if (value > MAX_BLINK_MS) value = MAX_BLINK_MS;

            g_target_ticks = value / TICK_MS;
            g_tick_count = 0;

            char msg[48];
            int len = snprintf(msg, sizeof(msg), "OK rate=%dms\r\n", value);
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, len, 20);
            return;
        }
    }

    char msg[] = "ERR unknown command\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, sizeof(msg) - 1, 20);
}

void App_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_UART_Receive_IT(&huart2, &g_rx_byte, 1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        g_tick_flag = 1;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        HAL_UART_Transmit(&huart2, &g_rx_byte, 1, 10);   // echo it back immediately

        if (g_rx_byte == '\r' || g_rx_byte == '\n') {
            if (g_cmd_index > 0) {
                g_cmd_buf[g_cmd_index] = '\0';
                Process_Command(g_cmd_buf);
                g_cmd_index = 0;
            }
        } else {
            if (g_cmd_index < CMD_BUF_SIZE - 1) {
                g_cmd_buf[g_cmd_index++] = (char)g_rx_byte;
            }
        }

        HAL_UART_Receive_IT(&huart2, &g_rx_byte, 1);
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