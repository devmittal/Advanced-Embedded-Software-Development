//*****************************************************************************
// @file    -   logger_task.h
// @brief   -   Function prototypes and macros for logger_task.c
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
//*****************************************************************************

#ifndef LOGGER_TASK_H_
#define LOGGER_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

QueueHandle_t xQueueTask1;
TaskHandle_t xAlertTask;

typedef struct temp{
    int temp_c;
    TickType_t cur_time;
}temp_t;

typedef struct led{
    int toggle_cnt;
    TickType_t cur_time;
    char *name;
}led_t;

typedef struct{
    temp_t temperature;
    led_t led;
    int id;
}mesg_t;

//*****************************************************************************
// @brief: Create logger Task
// @param: void
// @return: error status.
//*****************************************************************************
int logger_task_init(void);

//*****************************************************************************
// @brief: Convert ticks to millisecond
// @param: 1: tick count
// @return: converted millisecond value.
//*****************************************************************************
static inline int Tick_to_MS(TickType_t tick_time);

#endif /* LOGGER_TASK_H_ */
