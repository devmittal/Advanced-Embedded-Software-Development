/*
 * temp_task.h
 *
 *  Created on: Apr 6, 2019
 *      Author: Devansh
 */

#ifndef TEMP_TASK_H_
#define TEMP_TASK_H_

#define TEMP_FREQ (1000)
#define TMP102_ADDR (0x48)
#define SCALING_FACTOR  (0.0625)

extern uint32_t g_ui32SysClock;
uint8_t* pucRead2Byte(uint8_t slave_addr, uint8_t data);
int temp_task_init();

#endif /* TEMP_TASK_H_ */
