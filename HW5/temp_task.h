//*****************************************************************************
// @file    -   temp_task.h
// @brief   -   Function prototypes and macros for temp_task.c
// @author  -   Devansh Mittal
// @date    -   04/07/2019
// @version -   1.0
//*****************************************************************************

#ifndef TEMP_TASK_H_
#define TEMP_TASK_H_

#define TEMP_FREQ (1000)
#define TMP102_ADDR (0x48)
#define SCALING_FACTOR  (0.0625)
#define TEMP_THRESHOLD (27)

extern uint32_t g_ui32SysClock;

//*****************************************************************************
// @brief: Read 2 bytes via I2C
// @param: 1: slave address; 2: register address to be read from
// @return: pointer to data array read.
//*****************************************************************************
uint8_t* pucRead2Byte(uint8_t slave_addr, uint8_t data);

//*****************************************************************************
// @brief: Create Temperature Task
// @param: void
// @return: error status.
//*****************************************************************************
int temp_task_init(void);

#endif /* TEMP_TASK_H_ */
