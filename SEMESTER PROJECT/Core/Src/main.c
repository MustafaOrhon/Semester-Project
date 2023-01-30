/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmp180.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
float mytemperature,momenterytemp;
float minValueDay,maxValueDay,minValue,maxValue;
uint8_t myDeviceAdress=0x4E;
char my_str[17],my_str2[5], time[10],date[10],conc[25];
uint32_t a=0,pagecount=0;
int hal_start=1;
int dailyexample=0,page,offset;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
typedef enum {DisplayState_EraseEEPROM,DisplayState_MaxValueDay,DisplayState_MinValueDay,DisplayState_MinValue,DisplayState_MaxValue,DisplayState_Update} TDisplayState;
TDisplayState DisplayState= DisplayState_Update;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay(uint32_t time)
{
while(time--);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	mytemperature=momenterytemp;
	while(a!=0)
	{
		if(momenterytemp>maxValue)
		{
		maxValue=momenterytemp;
		}
	    if(momenterytemp<minValue)
		{
		minValue=momenterytemp;
		}
		if(momenterytemp>maxValueDay)
	    {
		maxValueDay=momenterytemp;
		}
		if(momenterytemp<minValueDay)
		{
		minValueDay=momenterytemp;
		}
        a=0;
	}

	if(dailyexample==2880)
	{
		maxValueDay=momenterytemp;
		minValueDay=momenterytemp;
		dailyexample=0;
	}
	a++;
	dailyexample++;
}
void set_time(void)
{
	  RTC_TimeTypeDef sTime = {0};
	  RTC_DateTypeDef sDate = {0};
	  /** Initialize RTC and set the Time and Date
	  */
	  sTime.Hours = 0x13;
	  sTime.Minutes = 0x30;
	  sTime.Seconds = 0x10;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
	  sDate.Month = RTC_MONTH_MAY;
	  sDate.Date = 0x31;
	  sDate.Year = 0x22;

	  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* USER CODE BEGIN RTC_Init 1 */
       HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1, 0x32F2);
	  /* USER CODE END RTC_Init 1 */
}

void get_time(void)
{
	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	HAL_RTC_GetTime(&hrtc,&gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&gDate, RTC_FORMAT_BIN);


	sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours,gTime.Minutes,gTime.Seconds);

	sprintf((char*)date,"%02d-%02d-%02d",gDate.Date,gDate.Month,gDate.Year+2000);
}
void display_time(void)
{
	  lcd_send_cmd(0x83);
	  lcd_send_string(date);
	  lcd_send_cmd(0xC0);
	  lcd_send_string(time);

}
void WriteToEEPROM(void)
{
	if(dailyexample%20==0)
	{
		sprintf(my_str2,"%.2f",mytemperature);
		strcpy(conc,my_str2);
		strcat(conc,time);
		strcat(conc,date);
		EEPROM_Write(page, offset, conc,23);
		offset+=23;
		pagecount++;
	}
	if(pagecount==2)
	{
		page++;
		offset=0;
		pagecount=0;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  BMP180_Init();
  lcd_init();
  lcd_send_cmd(0x01);
  if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)  != 0x32F2)
   {
	 set_time();
   }
  mytemperature=BMP180_Get_Temperature();
  minValue=mytemperature;
  minValueDay=mytemperature;
  maxValue=mytemperature;
  maxValueDay=mytemperature;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



	        switch(DisplayState)
	        {
	         case DisplayState_EraseEEPROM:
	        	lcd_clear();
	        	delay(1680000);
	        	lcd_send_cmd(0x81);
	        	lcd_send_string("EEPROM ERASING");
	        	lcd_send_cmd(0xC2);
	        	lcd_send_string("WAIT PLEASE");
	        	for(int i=0;i<512;i++)
	        	{
	        		EEPROM_PageErase(i);
	        	}
	        	lcd_clear();
	        	DisplayState=DisplayState_Update;
	        	break;

	        case DisplayState_MaxValue:
	        	    lcd_clear();
	        	    delay(1680000);
	        	    lcd_send_cmd(0x81);
	        	    lcd_send_string("HighValueOfAll");
	              	sprintf(my_str,"%.2f",maxValue);
	            	lcd_send_cmd(0xC9);
	            	lcd_send_string(my_str);
	            	delay(56800000);
	                lcd_clear();
	            	DisplayState=DisplayState_Update;
	        	    break;

	        case DisplayState_MinValue:
	   	        	lcd_clear();
	   	        	delay(1680000);
	   	        	lcd_send_cmd(0x81);
	   	        	lcd_send_string("MinValueOfAll");
	   	        	sprintf(my_str,"%.2f",minValue);
	   	        	lcd_send_cmd(0xC9);
	   	        	lcd_send_string(my_str);
	   	        	delay(56800000);
	   	            lcd_clear();
	   	        	DisplayState=DisplayState_Update;
	   	        	break;

	        case DisplayState_MinValueDay:
	       	   	    lcd_clear();
	   	        	delay(1680000);
	   	        	lcd_send_cmd(0x81);
	       	   	    lcd_send_string("HighValueDay");
	       	   	    sprintf(my_str,"%.2f",maxValueDay);
	       	   	    lcd_send_cmd(0xC9);
	       	   	    lcd_send_string(my_str);
	       	   	    delay(56800000);
	       	   	    lcd_clear();
	       	   	    DisplayState=DisplayState_Update;
	       	   	    break;

	        case DisplayState_MaxValueDay:
	       	   	    lcd_clear();
	       	   	    delay(1680000);
	       	   	    lcd_send_cmd(0x81);
	       	   	    lcd_send_string("MinValueDay");
	       	   	    sprintf(my_str,"%.2f",minValueDay);
	       	   	    lcd_send_cmd(0xC9);
	       	   	    lcd_send_string(my_str);
	       	   	    delay(56800000);
	       	   	    lcd_clear();
	       	   	    DisplayState=DisplayState_Update;
	       	   	    break;

	         default :
	        	    get_time();
		            sprintf(my_str,"%.2f",mytemperature);
                    display_time();
                    lcd_send_cmd(0xC9);
 	                lcd_send_string(my_str);
 	                momenterytemp=BMP180_Get_Temperature();
                    if(hal_start==1)
                  {
            	    HAL_TIM_Base_Start_IT(&htim2);
            	    hal_start=0;
                  }
                    WriteToEEPROM();

	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{


  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }



}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
