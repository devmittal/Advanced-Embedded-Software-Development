//*****************************************************************************
// @file    -   led_task.h
// @brief   -   Function prototypes and macros for led_task.c
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
//*****************************************************************************

#ifndef __LED_TASK_H__
#define __LED_TASK_H__

#define LED_FREQ (50)

//*****************************************************************************
//
// Prototypes for the LED task.
//
//*****************************************************************************

int led_task_init(void);

//*****************************************************************************
// @brief: Toggle LED based on parameter. Send toggle count, name and timestamp
//         to logger task via queue.
// @param: 1: LED on/off
// @return: void
//*****************************************************************************
void control_led(int status);

#endif // __LED_TASK_H__
