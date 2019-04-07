//*****************************************************************************
// @file    -   temp_task.c
// @brief   -   Creates logger task. Task blocks to receive messages from temperature
//              logger, and alert task, and prints the messages to UART.
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "logger_task.h"

static inline int Tick_to_MS(TickType_t tick_time)
{
    return (int)((tick_time*(TickType_t)1000)/(TickType_t)configTICK_RATE_HZ);
}

static void logger(void *pvParameters)
{
    mesg_t message;
    int timestamp;

    while(1)
    {
        xQueueReceive(xQueueTask1,&message,portMAX_DELAY);

        /* message from temperature task */
        if(message.id == 0)
        {
            timestamp = Tick_to_MS(message.temperature.cur_time);
            UARTprintf("\n[%d ms]Temperature: %d.%d celsius", timestamp, (message.temperature.temp_c/1000),
                           (message.temperature.temp_c%1000));
        }

        /* message from LED task */
        else if(message.id == 1)
        {
            timestamp = Tick_to_MS(message.led.cur_time);
            UARTprintf("\n[%d ms]Name: %s. Toggle Count = %d", timestamp, (message.led.name),
                       (message.led.toggle_cnt));
        }

        /* message from alert task */
        else
            UARTprintf("\nAlert: Temperature out of normal range: %d.%d celsius", (message.temperature.temp_c/1000),
                       (message.temperature.temp_c%1000));
    }
}

int logger_task_init()
{
    if(xTaskCreate(logger,"Logger task",1000,NULL,(configMAX_PRIORITIES-2),NULL) == pdFAIL)
        return 0;

    return 1;
}
