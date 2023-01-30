/*
 * bmp180.h
 *
 *  Created on: May 22, 2022
 *      Author: Mustafa
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_
#include "stm32f4xx_hal.h"
extern  I2C_HandleTypeDef hi2c1;

#define BMP_180_CALIBRATION_VALUE_LENGTH 22


#define BMP180_DEVICE_WRITE_REGISTER_ADDRESS 0XEE //BMP180 WRITING ADDRESS
#define BMP180_DEVICE_READ_REGISTER_ADDRESS 0XEF //BMP180 READING ADDRESS
#define BMP180_CALIBRATION_START_ADDRESS 0XAA




void BMP180_Init();
void BMP180_GetCalibration();
void BMP180_Get_Calibration_Value();
void BMP180_Get_Uncompansated_Temperature(void);
float BMP180_Get_Temperature(void);



#endif /* INC_BMP180_H_ */
