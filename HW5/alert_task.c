//*****************************************************************************
// @file    -   led_task.c
// @brief   -   Creates alert task. Waits for notification. If notification
//              received, send notification value via queue to logger task
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
//*****************************************************************************

#include "alert_task.h"
#include "logger_task.h"
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

static void alert(void *pvParameters)
{
    uint32_t ulAlertTemp;
    mesg_t alert_cur;

    while(1)
    {
        xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
                         ULONG_MAX, /* Reset the notification value to 0 on exit. */
                         &ulAlertTemp, /* Notified value pass out in ulAlertTemp. */
                         portMAX_DELAY );  /* Block indefinitely. */

        alert_cur.temperature.temp_c = ulAlertTemp;
        alert_cur.id = 2;

        xQueueSendToFront(xQueueTask1,&alert_cur,portMAX_DELAY);
    }
}

int alert_task_init(void)
{
    if(xTaskCreate(alert,"Alert task",1000,NULL,(configMAX_PRIORITIES-2),&xAlertTask) == pdFAIL)
        return 0;

    return 1;
}

