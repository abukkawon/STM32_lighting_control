/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "eth.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void impulsator();
void photoresistor();
float calc_pwm(float val);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart3;
uint16_t photoresistor_B;
uint16_t impulsator_B;
uint16_t photoresistor_100;
uint16_t impulsator_100;
// IMPULSATOR
float max = 1980; //2440
float min = 0;
float min_VOLT = 0;
float max_VOLT = 2;
float impulsator_V;
float impulsator_M;
float photoresistor_V;
_Bool state;
_Bool usb;
char strimp;
char strosw;
char *struart;
int start = 0;
int value;
float Perror = 0;
float Ierror = 0;
float Derror = 0;
float LastError = 0;
int setpoint;
float Pout;
float Iout;
float Dout;
float output;
float Kp = 0.012;
float Ki = 0.006;
float Kd = 0.00015;
float result;
int a = 0;
uint8_t old[4];
uint8_t received[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  lcd_init();
  HAL_UART_RxCpltCallback(&huart3);
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	  //funkcje odczytujące wartości z wejsć analogowych
	  impulsator();
	  photoresistor();

	  //zmienne z informacją o stanie wejść informujących o podłączeniu kabla oraz wybraniu przez użytkownika uart/potencjometr
	  state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
	  usb =  HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);

	  // Odczyt danych z Uarta i przypisanie ich do jednej zmiennej liczbowej
	  if(memcmp(received, old, sizeof(received)) != 0){

								if(received[start]== '1'){

		  	  	  	  	  	  		value = received[((start + 1)%4)] - 48;
		  	  	  	  	  	  		start = (start + 2)%4;

		              	  		}
		              	  		if(received[start]== '2'){

		              	  			value = (received[((start + 1)%4)] - 48)*(10) + (received[((start + 2)%4)] - 48);
		              	  			start = (start + 3)%4;

		              	  		}
		              	  		if(received[start]== '3'){

		              	  			value = (received[((start + 1)%4)] - 48)*(100) + (received[((start + 2)%4)] - 48)*(10) + (received[((start + 3)%4)] - 48);
		              	  				start = (start + 4)%4;
		              	  		}
		              	  		for(int i = 0; i < strlen(received); i++){
		              	  			old[i] = received[i];
		              	  		}
		              	  		if(value > 100){
		              	  			value = value % 100;
		              	  		}
	  }
	  // wybór uart/potencjometr
	  if (state || !usb)
	                  {
		  	  	  	  	  	  	 if(a == 1){
		  	  	  	  	  	  		 HAL_Delay(1000);
		  	  	  	  	  	  		 lcd_init();
		  	  	  	  	  	  		 a = 0;
		  	  	  	  	  	  	 }
		  	  	  	  	  	  	 setpoint = impulsator_B;
		  	  	  	  	  	  	 lcd_put_cur(0, 0);
		  	  	  	  	  	  	 lcd_send_string("Impulsator:");
		  	  	  	  	  	  	 strimp="";
		  	  	  	  	  	  	 sprintf(strimp,"%d",impulsator_100);
		  	  	  	  	  	  	 if(strlen(strimp)==1){
		  	  	  	  	  	  		 lcd_put_cur(0, 12);
		  	  	  	  	  	  		 lcd_send_string(strimp);
		  	  	  	  	  	  		 lcd_put_cur(0, 13);
		  	  	  	  	  	  		 lcd_send_string("%  ");
		  	  	  	  	  	  	 }
		  	  	  	  	  	  	 if(strlen(strimp)==2){
		  	  	  	  	  	  		 lcd_put_cur(0, 12);

		  	  	  	  	  	  		 lcd_send_string(strimp);
		  	  	  	  	  	  		 lcd_put_cur(0, 14);
		  	  	  	  	  	  		 lcd_send_string("% ");
		  	  	  	  	  	  	 }
		  	  	  	  	  	  	 if(strlen(strimp)==3){
		  	  	  	  	  	  		 lcd_put_cur(0, 12);
		  	  	  	  	  	  		 lcd_send_string(strimp);
		  	  	  	  	  	  		 lcd_put_cur(0, 15);
		  	  	  	  	  	  		 lcd_send_string("%");
		  	  	  	  	  	  	 }
		  	  	  	  	  	  	 a = 0;
	  }else if(!state && usb){
		  	  	  	  	  	  	  if(a == 0){
		  	  	  	  	  	  		  HAL_Delay(1000);
		  	  	  	  	  	  		  lcd_init();
		  	  	  	  	  	  		  a = 1;
		  	  	  	  	  	  	  }
		  	  	  	  	  	  	  setpoint = (int)((value + 0.5) * max / 100);
		  	  	  	  	  	  	  lcd_put_cur(0, 0);
		  	  	  	  	  	  	  lcd_send_string("Uart:       ");
		  	  	  	  	  	  	  sprintf(struart,"%d", value);

		  	  	  	  	  	  	  if(strlen(struart)==1){
		  	  	  	  	  	  		  lcd_put_cur(0, 12);
		  	  	  	  	  	  		  lcd_send_string(struart);
		  	  	  	  	  	  		  lcd_put_cur(0, 13);
		  	  	  	  	  	  		  lcd_send_string("%  ");
		  	  	  	  	  	  	  }
		  	  	  	  	  	  	  if(strlen(struart)==2){
		  	  	  	  	  	  		  lcd_put_cur(0, 12);
		  	  	  	  	  	  		  lcd_send_string(struart);
		  	  	  	  	  	  		  lcd_put_cur(0, 14);
		  	  	  	  	  	  		  lcd_send_string("% ");
		  	  	  	  	  	  	  }
		  	  	  	  	  	  	  if(strlen(struart)==3){
		  	  	  	  	  	  		  lcd_put_cur(0, 12);
		  	  	  	  	  	  		  lcd_send_string(struart);
		  	  	  	  	  	  		  lcd_put_cur(0, 15);
		  	  	  	  	  	  		  lcd_send_string("%");
		  	  	  	  	  	  	  }
	  }
	  lcd_put_cur(1, 0);
	  lcd_send_string("Oswietlenie:");
	  strosw="";
	  sprintf(strosw,"%d",photoresistor_100);
	  if(strlen(strosw)==1){
		  lcd_put_cur(1, 12);
		  lcd_send_string(strosw);
		  lcd_put_cur(1, 13);
		  lcd_send_string("%  ");
	  }
	  if(strlen(strosw)==2){
		  lcd_put_cur(1, 12);
		  lcd_send_string(strosw);
		  lcd_put_cur(1, 14);
		  lcd_send_string("% ");
	  }
	  if(strlen(strosw)==3){

		  lcd_put_cur(1, 12);
		  lcd_send_string(strosw);
		  lcd_put_cur(1, 15);
		  lcd_send_string("%");
	  }

	  HAL_Delay(50);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void impulsator()
{
          impulsator_B = HAL_ADC_GetValue(&hadc2) * max / 2450;
          impulsator_M = (int)((int)(impulsator_B / max * 100) + 0.5)*max/100.0;
          impulsator_V =(float)(impulsator_B-min)/(max-min)*(max_VOLT-min_VOLT);
          impulsator_100 = (int)((impulsator_V)/(max_VOLT-min_VOLT)*100);
          HAL_ADC_Start(&hadc1);
}

void photoresistor()
{
      photoresistor_B = HAL_ADC_GetValue(&hadc1);
      photoresistor_V =(float)(photoresistor_B-min)/(max-min)*(max_VOLT-min_VOLT);
      photoresistor_100 = (float)(photoresistor_V)/(max_VOLT-min_VOLT)*100;
      HAL_ADC_Start(&hadc2);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(&huart3, &received, 4);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	Perror = setpoint - photoresistor_B;
	Ierror = Ierror + ((Perror + LastError)*0.005*0.5);
	Derror = (Perror - LastError)/0.005;
	Pout = Kp * Perror;
	Iout = Ki * Ierror;
	Dout = Kd * Derror;

	if(Iout > 2440){
		Iout = 2440;
	}
	if(Iout < 0){
		Iout = 0;
	}
	if(output + (Pout + Iout + Dout)<=2440 && output + ((Pout + Iout + Dout))>=0){
	output = output + (Pout + Iout + Dout);
	}
	if(output > 2440){
		output = 2440;
	}
	if(output < 0){
		output = 0;
	}
	LastError = Perror;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 1000 - output );

}
float calc_pwm(float val)
{
    const float k = 0.1f;
    const float x0 = 60.0f;
    result = 500.0f / (1.0f + exp(-k * ((val) - x0)));
    return result;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
