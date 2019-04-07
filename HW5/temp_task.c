/*
 * temp_task.c
 *
 *  Created on: Apr 6, 2019
 *      Author: Devansh
 */

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
#include "temp_task.h"
#include "logger_task.h"
#include "driverlib/i2c.h"

uint8_t* pucRead2Byte(uint8_t slave_addr, uint8_t data)
{
    static uint8_t rcv_data[2] = {0};

    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);
    I2CMasterDataPut(I2C2_BASE, data);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, true);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));
    rcv_data[0] = I2CMasterDataGet(I2C2_BASE);

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));
    rcv_data[1] = I2CMasterDataGet(I2C2_BASE);

    return rcv_data;
}

static void temperature(void *pvParameters)
{
    uint8_t* i2c_data_rcv;
    int celsius;
    int16_t temperature_hex = 0;
    TickType_t xWakeTime;
    TickType_t timestamp;
    mesg_t temp_cur;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);

    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);
    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);

    I2CMasterInitExpClk(I2C2_BASE, g_ui32SysClock, false);

    xWakeTime = xTaskGetTickCount();

    while(1)
    {
        timestamp = xTaskGetTickCount();
        i2c_data_rcv = pucRead2Byte(TMP102_ADDR, 0);

        temperature_hex = 0;
        temperature_hex = (((*i2c_data_rcv) << 8) | *(i2c_data_rcv+1)) >> 4;

        if(((temperature_hex & (0x0800)) >> 11))
        {
            temperature_hex ^= (0x0FFF);
            temperature_hex += 1;
            temperature_hex = temperature_hex * (-1);
        }
        celsius = temperature_hex * SCALING_FACTOR * 1000;

        temp_cur.temperature.temp_c = celsius;
        temp_cur.temperature.cur_time = timestamp;
        temp_cur.id = 0;

        xQueueSendToFront(xQueueTask1,&temp_cur,portMAX_DELAY);

       // UARTprintf("\nTemperature: %d.%d", (celsius/1000),(celsius%1000));

        vTaskDelayUntil(&xWakeTime, pdMS_TO_TICKS(TEMP_FREQ));
    }
}

int temp_task_init()
{
    if(xTaskCreate(temperature,"Temperature Read task",1000,NULL,(configMAX_PRIORITIES-2),NULL) == pdFAIL)
        return 0;

    return 1;
}
