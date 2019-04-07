//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2009-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the DK-TM4C129X Firmware Package.
//
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

//*****************************************************************************
//
// This task simply toggles the user LED at a 1 Hz rate.
//
//*****************************************************************************
void control_led(int status)
{
    TickType_t timestamp;
    mesg_t led_cur;
    static int toggle_count = 0;

    timestamp = xTaskGetTickCount();
    if(status == 1)
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    }
    else
    {
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
        ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }

    toggle_count++;

    led_cur.id = 1;
    led_cur.led.cur_time = timestamp;
    led_cur.led.toggle_cnt = toggle_count;
    led_cur.led.name = "Devansh";

    xQueueSendToFront(xQueueTask1,&led_cur,portMAX_DELAY);
}

static void led(void *pvParameters)
{
    TickType_t xLastTime;

    // Get the current tick count.
    xLastTime = xTaskGetTickCount();

    // Loop forever.
    while(1)
    {
        control_led(1);

        // Wait for the required amount of time.
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(LED_FREQ));

        control_led(0);

        // Wait for the required amount of time.
        vTaskDelayUntil(&xLastTime, pdMS_TO_TICKS(LED_FREQ));
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
int led_task_init(void)
{
    //
    // Initialize the GPIO used to drive the user LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    //
    // Create the LED task.
    //
	if(xTaskCreate(led,"LED task",1000,NULL,(configMAX_PRIORITIES-2),NULL) == pdFAIL)
        return 0;

    return 1;
}
