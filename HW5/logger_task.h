/*
 * logger_task.h
 *
 *  Created on: Apr 6, 2019
 *      Author: Devansh
 */

#ifndef LOGGER_TASK_H_
#define LOGGER_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

QueueHandle_t xQueueTask1;

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

int logger_task_init();
static inline int Tick_to_MS(TickType_t tick_time);

#endif /* LOGGER_TASK_H_ */
