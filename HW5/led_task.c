//*****************************************************************************
// @file    -   led_task.c
// @brief   -   Creates led task. Toggles D1 and D2 LED every 100ms. Sends toggle
//              count, name and timestamp via queue to logger task
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
#include "driverlib/sysctl.h"
#include "led_task.h"
#include "logger_task.h"

static void led(void *pvParameters)
{
    TickType_t xLastTime;
    TickType_t timestamp;
    mesg_t led_cur;
    static int toggle_count = 0;

    // Get the current tick count.
    xLastTime = xTaskGetTickCount();

    // Loop forever.
    while(1)
    {
        // Wait for the required amount of time.
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(LED_FREQ));

        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); //Switch on LED

        // Wait for the required amount of time.
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(LED_FREQ));

        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0); //Switch off LED

        timestamp = xTaskGetTickCount();

        toggle_count++;

        led_cur.id = 1;
        led_cur.led.cur_time = timestamp;
        led_cur.led.toggle_cnt = toggle_count;
        led_cur.led.name = "Devansh";

        xQueueSendToFront(xQueueTask1,&led_cur,portMAX_DELAY);
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
int led_task_init(void)
{
    // Initialize the GPIO used to drive the user LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    // Create the LED task.
	if(xTaskCreate(led,"LED task",1000,NULL,(configMAX_PRIORITIES-2),NULL) == pdFAIL)
        return 0;

    return 1;
}
