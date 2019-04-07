//*****************************************************************************
// @file    -   main.c
// @brief   -   Calls functions to create 4 tasks - temperature, LED, logger and
//              alert task
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
// @references - freertos_demo project found under TivaWare_C_Series-2.1.4.178\
//               examples\boards\dk-tm4c129x\freertos_demo used as the foundation
//               for the project.
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
#include "alert_task.h"

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

//*****************************************************************************
//
// Configure UART
//
//*****************************************************************************
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
    // Run from the PLL at 120 MHz.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            configCPU_CLOCK_HZ);

    InitConsole();

    UARTprintf("\nAESD. HW5");

    /* Create Queue */
    xQueueTask1 = xQueueCreate(10,sizeof(mesg_t));
    if(xQueueTask1 == NULL)
    {
        UARTprintf("\nQueue not created");
        while(1);
    }

    /* Create temperature task */
    if(temp_task_init() == 0)
    {
        UARTprintf("\nTemp task not created");
        while(1);
    }

    /* Create logger task */
    if(logger_task_init() == 0)
    {
        UARTprintf("\nLogger task not created");
        while(1);
    }

    /* Create LED task */
    if(led_task_init() == 0)
    {
        UARTprintf("\nLED task not created");
        while(1);
    }

    /* Create alert task */
    if(alert_task_init() == 0)
    {
        UARTprintf("\nAlert task not created");
        while(1);
    }

    vTaskStartScheduler();

    // In case the scheduler returns for some reason, loop forever.
    while(1)
    {
    }
}
