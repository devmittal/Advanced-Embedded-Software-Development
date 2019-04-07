//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
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
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.c"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "utils/random.h"
#include "temp_task.h"
#include "logger_task.h"
#include "led_task.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application utilizes FreeRTOS to perform a variety of tasks in a
//! concurrent fashion.  The following tasks are created:
//!
//! * An lwIP task, which serves up web pages via the Ethernet interface.  This
//!   is actually two tasks, one which runs the lwIP stack and one which
//!   manages the Ethernet interface (sending and receiving raw packets).
//!
//! * An LED task, which simply blinks the on-board LED at a user-controllable
//!   rate (changed via the web interface).
//!
//! * A set of spider tasks, each of which controls a spider that crawls around
//!   the LCD.  The speed at which the spiders move is controllable via the web
//!   interface.  Up to thirty-two spider tasks can be run concurrently (an
//!   application-imposed limit).
//!
//! * A spider control task, which manages presses on the touch screen and
//!   determines if a spider task should be terminated (if the spider is
//!   ``squished'') or if a new spider task should be created (if no spider is
//!   ``squished'').
//!
//! * There is an automatically created idle task, which monitors changes in
//!   the board's IP address and sends those changes to the user via a UART
//!   message.
//!
//! Across the bottom of the LCD, several status items are displayed:  the
//! amount of time the application has been running, the number of tasks that
//! are running, the IP address of the board, the number of Ethernet packets
//! that have been transmitted, and the number of Ethernet packets that have
//! been received.
//!
//! The finder application (in tools/finder) can also be used to discover the
//! IP address of the board.  The finder application will search the network
//! for all boards that respond to its requests and display information about
//! them.
//!
//! The web site served by lwIP includes the ability to adjust the toggle rate
//! of the LED task and the update speed of the spiders (all spiders move at
//! the same speed).
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//!
//! For additional details on lwIP, refer to the lwIP web page at:
//! http://savannah.nongnu.org/projects/lwip/
//
//*****************************************************************************

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    // Fatal RTOS error
    while(1)
    {
    }
}

void InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            configCPU_CLOCK_HZ);

    InitConsole();

    UARTprintf("\nTesting UART. Pls work");

    xQueueTask1 = xQueueCreate(10,sizeof(mesg_t));
    if(xQueueTask1 == NULL)
    {
        UARTprintf("\nQueue not created");
        while(1);
    }

    if(temp_task_init() == 0)
    {
        UARTprintf("\nTemp task not created");
        while(1);
    }

    if(logger_task_init() == 0)
    {
        UARTprintf("\nLogger task not created");
        while(1);
    }

    if(led_task_init() == 0)
    {
        UARTprintf("\nLED task not created");
        while(1);
    }

    vTaskStartScheduler();

    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //
    while(1)
    {
    }
}
