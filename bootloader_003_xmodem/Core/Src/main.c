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
#include"bootloader.h"
#include"xmodem.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CMD_BUF_SIZE 20
#define PRINT_MENU

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart4;//debug UART
UART_HandleTypeDef huart5;		//xmodem UART

/* USER CODE BEGIN PV */
uint8_t cmdBuf[CMD_BUF_SIZE];
//bl_sig_t __attribute__((section(".sigsection"))) signature;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART5_Init(void);
static void MX_UART4_Init(void);
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
//	bootloader_unlock_flash();
//	bootloader_flash_erase_signature_area();
//	bl_sig_t temp={.app_version=1,.bl_version=1,.update_flag=1};
//	memcpy(FLASH_SIGNATURE_AREA,&temp,sizeof(bl_sig_t));
//	bootloader_lock_flash();
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
  MX_UART5_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /*Enter Bootloader mode when User_button is pressed*/
  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET){
	  bootloader_mode();
  }
  else{
	  uint8_t send=ACK,receive=0;
	  HAL_UART_Transmit(&huart4, "checking Update\n\r", strlen("checking Update\n\r"),HAL_MAX_DELAY);

	  /*wait for reception of ACK or NAK*/
	  while(HAL_UART_Receive(&huart5, &receive, 1, 1000)!=HAL_OK){
		  //send ACK
		  HAL_UART_Transmit(&huart5,&send, 1, HAL_MAX_DELAY);
	  }

	  if(receive==ACK)
	  {
		  //update available
		  HAL_UART_Transmit(&huart4, "Downloading Update\n\r", strlen("Downloading Update\n\r"),HAL_MAX_DELAY);
		  /*erase download area*/
		  bootloader_flash_erase_download_area();
		  /*receive file using xmodem*/
		  while(xmodem_receive(&huart5)!=XMODEM_ERROR);
		  /*if any error happens in xmodem*/
		  HAL_UART_Transmit(&huart4, "Downloading Update\n\r", strlen("Downloading failed\n\r"),HAL_MAX_DELAY);

	  }
	  else
	  {
	  //if received NAK
	  HAL_UART_Transmit(&huart4, "Update NOT available\n\r", strlen("Update NOT available\n\r"),HAL_MAX_DELAY);
	  bootloader_jump_to_user_code(&huart4);
	  }
}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void bootloader_mode(){

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, SET);
	/*Use UART5 for bootloader command*/
#ifdef PRINT_MENU
	HAL_UART_Transmit(&huart4,"Welcome to bootloader\r\n", strlen("Welcome to bootloder\r\n"),HAL_MAX_DELAY );
	HAL_UART_Transmit(&huart4,"0 BL_JMP_TO_USER_CODE\r\n", strlen("0 BL_JMP_TO_USER_CODE\r\n"),HAL_MAX_DELAY );
	HAL_UART_Transmit(&huart4,"1 BL_WRITE_BIN_TO_MEMORY\r\n", strlen("1 BL_WRITE_BIN_TO_MEMORY\r\n"),HAL_MAX_DELAY );
	HAL_UART_Transmit(&huart4,"2 BL_GET_VERSION\r\n", strlen("2 BL_GET_VERSION\r\n"),HAL_MAX_DELAY );
#endif
	/*poll UART5 to read data*/
	while(1)
	{
		/*Receive command from host*/
		HAL_UART_Receive(&huart5, cmdBuf, CMD_SIZE, HAL_MAX_DELAY);

		//break through cmd
		switch(cmdBuf[0])
		{
		case BL_GET_VERSION:
			bootloader_get_bl_version(&huart5);
			break;
		case BL_JMP_TO_USER_CODE:
			bootloader_jump_to_user_code(&huart4);
			break;
		case BL_WRITE_BIN_TO_MEMORY:
#ifdef PRINT_MENU
			HAL_UART_Transmit(&huart4,(uint8_t*) "xmodem mode\n\r", strlen("xmodem mode\n\r"),HAL_MAX_DELAY );
#endif
			bootloader_flash_erase_download_area();
			while(1){
				if(xmodem_receive(&huart5)==XMODEM_ERROR){
					HAL_UART_Transmit(&huart4, "ERROR", strlen("ERROR"), HAL_MAX_DELAY);
					while(1);
				}
			}
			break;
		default:
			HAL_UART_Transmit(&huart4, "Invalid cmd\n\r", strlen("Invalid cmd\n\r"), HAL_MAX_DELAY);
		}
	}
}


void glow_all_led(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, SET);

}
void reset_all_led(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, RESET);

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
