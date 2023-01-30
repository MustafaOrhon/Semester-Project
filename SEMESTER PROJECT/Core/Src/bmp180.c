	/*
	 * bmp180.c
	 *
	 *  Created on: May 22, 2022
	 *      Author: Mustafa
	 */
	#include "bmp180.h"
	#include "stm32f4xx_hal.h"
	extern  I2C_HandleTypeDef hi2c1;
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	int16_t AC4;
	int16_t AC5;
	int16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
	int16_t unCompTemperature;
	float Temperature;
	int32_t tX1,tX2,tB5;

	void BMP180_Get_Calibration_Value()
	{
	   int a=0;
	   uint8_t calibBuff[BMP_180_CALIBRATION_VALUE_LENGTH]= {0}  ;

	   HAL_I2C_Mem_Read(&hi2c1, BMP180_DEVICE_READ_REGISTER_ADDRESS,BMP180_CALIBRATION_START_ADDRESS,1, calibBuff, 22, 100);

		 AC1 = (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 AC2 = (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 AC3= (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 AC4= (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 AC5= (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 AC6= (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 B1=  (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 B2=  (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 MB=  (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 MC=  (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;
		 MD=  (int16_t)((calibBuff[a]<<8) | calibBuff[a+1]); a+=2;

		 if((AC1==0x00 )| (AC1==0xFFFF))
		 {
			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
			 while(1);
		 }
		 if((AC2==0x00) | (AC2==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((AC3==0x00 )| (AC3==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((AC4==0x00) | (AC4==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((AC5==0x00) | (AC5==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((AC6==0x00) | (AC6==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((B2==0x00 )| (B2==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if((MB==0x00) | (MB==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }

		 if( (MC==0x00) | (MC==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }
		 if((MD==0x00 )| ( MD==0xFFFF))
		 {
				 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
				 while(1);
			 }




	}
	void BMP180_Init()
	{

		if(HAL_I2C_IsDeviceReady(&hi2c1, BMP180_DEVICE_WRITE_REGISTER_ADDRESS, 1, 100000)!= HAL_OK)
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,SET);

		BMP180_Get_Calibration_Value();

	}
	void BMP180_Get_Uncompansated_Temperature()
	{
		uint8_t rData[2]={0};
		uint8_t wData[1];
		wData[0]=0x2E;
	HAL_I2C_Mem_Write(&hi2c1,BMP180_DEVICE_WRITE_REGISTER_ADDRESS, 0xF4,1,wData,1,100000);
	HAL_Delay(5);

	HAL_I2C_Mem_Read(&hi2c1, BMP180_DEVICE_READ_REGISTER_ADDRESS,0xF6, 1, rData, 2, 100000);

	unCompTemperature=(int16_t)((rData[0]<<8) | rData[1]);


	}
	float BMP180_Get_Temperature()
	{
	  BMP180_Get_Uncompansated_Temperature();
	  tX1=((int32_t)unCompTemperature - (int32_t)AC6) * (int32_t)AC5 / 32768;
	  tX2=((int32_t)MC * 2048) / (tX1 + (int32_t)MD);

	  if((tX1==0 )|(tX2==0))
	  {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
		 while(1);
	  }

	  tB5=tX1+tX2;
	  Temperature= (float) (tB5+8) / 16 * 0.1;
	  return Temperature;

	}



