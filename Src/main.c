/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
#include "esp8266_at.h"
#include "esp8266_mqtt.h"
#include "string.h"
#include "stdio.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define WIFI_NAME "LIANG"
#define WIFI_PASSWD "liang0526"

#define MQTT_BROKERADDRESS "a16OKk6dTya.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_CLIENTID "12345|securemode=3,signmethod=hmacsha1|"
#define MQTT_USARNAME "SAMA&a16OKk6dTya"
#define MQTT_PASSWD "04629D74064DC8ED55CE7656722490094705C38D"
#define	MQTT_PUBLISH_TOPIC "/sys/a16OKk6dTya/KAMI/thing/event/property/post"
//#define MQTT_SUBSCRIBE_TOPIC "/sys/a16OKk6dTya/KAMI/thing/event/property/post_reply"
#define MQTT_SUBSCRIBE_TOPIC "/a16OKk6dTya/SAMA/user/get"

#define LOOPTIME 30
#define HEARTBEAT   (5000/LOOPTIME)

#define USER_MAIN_DEBUG

#ifdef USER_MAIN_DEBUG
#define user_main_printf(format, ...) printf( format "\r\n",##__VA_ARGS__)
#define user_main_info(format, ...) printf("【main】info:" format "\r\n",##__VA_ARGS__)
#define user_main_debug(format, ...) printf("【main】debug:" format "\r\n",##__VA_ARGS__)
#define user_main_error(format, ...) printf("【main】error:" format "\r\n",##__VA_ARGS__)
#else
#define user_main_printf(format, ...)
#define user_main_info(format, ...)
#define user_main_debug(format, ...)
#define user_main_error(format, ...)
#endif
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

/* USER CODE BEGIN PV */
extern uint8_t usart2_txbuf[256];
extern uint8_t usart2_rxbuf[512];
extern uint8_t usart2_rxone[1];
extern uint8_t usart2_rxcounter;
char mqtt_message[300];	//MQTT的上报消息缓存
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Enter_ErrorMode(uint8_t mode);
void MQTT_SendBuf(uint8_t *buf,uint16_t len);
uint8_t Find_Str(char* dest,char* src,uint16_t retry_nms);
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
HAL_UART_Receive_IT(&huart2,usart2_rxone,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  /* USER CODE END 3 */
  while(1)
  {
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
}

/* USER CODE BEGIN 4 */
void Enter_ErrorMode(uint8_t mode)
{
	while(1)
	{
		switch(mode){
			case 0:user_main_error("ESP8266初始化失败！\r\n");break;
			case 1:user_main_error("ESP8266连接热点失败！\r\n");break;
			case 2:user_main_error("ESP8266连接阿里云服务器失败！\r\n");break;
			case 3:user_main_error("ESP8266阿里云MQTT登陆失败！\r\n");break;
			case 4:user_main_error("ESP8266阿里云MQTT订阅主题失败！\r\n");break;
			default:user_main_info("Noting\r\n");break;
		}
		user_main_info("请重启开发板");
		HAL_Delay(200);
	}
}

void KEY0_Pressed(void)
{
	user_main_debug("我按下了KEY_0\r\n");
	uint8_t status=0;
	if(ESP8266_Init())
	{
		user_main_info("ESP8266初始化成功！\r\n");
		status++;
	}
	else Enter_ErrorMode(0);
	if(status==1)
	{
		if(ESP8266_ConnectAP(WIFI_NAME,WIFI_PASSWD))
		{
			user_main_info("ESP8266连接热点成功！\r\n");
			status++;
		}
		else Enter_ErrorMode(1);
	}
	if(status==2)
	{
		if(ESP8266_ConnectServer("TCP",MQTT_BROKERADDRESS,1883)!=0)
		{
			user_main_info("ESP8266连接阿里云服务器成功！\r\n");
			status++;
		}
		else Enter_ErrorMode(2);
	}
	if(status==3)
	{
		if(MQTT_Connect(MQTT_CLIENTID,MQTT_USARNAME,MQTT_PASSWD)!=0)
		{
			user_main_info("ESP8266阿里云MQTT登陆成功！\r\n");
			status++;
//			while (1)
//			{
//    /* USER CODE END WHILE */
//	  sprintf(mqtt_message,
//	"{\"params\":{\"ParkingState\":1}}");
//	  MQTT_PublishData(MQTT_PUBLISH_TOPIC,mqtt_message,0);
//				HAL_Delay(1000);
//			}
		}
		else Enter_ErrorMode(3);
	}
	if(status==4)
	{
		if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1)!=0)
		{
			user_main_info("ESP8266阿里云MQTT订阅主题成功！\r\n");
			while(1)
			{
				uint16_t count=0;
				//char ch1[11]="a16OKk6dTya";
				//char ch2[2]=":0";
				while(count++<usart2_rxcounter)
					printf("%u:%c",count,usart2_rxbuf[count]);
				//if(Find_Str((char*)usart2_rxbuf,(char *)ch1,2000) != 0)
				if((char)usart2_rxbuf[48]=='1'){
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
					printf("I GOT IT!");
					HAL_Delay(1000);
			}
				if((char)usart2_rxbuf[48]=='0'){
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
					printf("I GOT IT!");
					HAL_Delay(1000);
			}
//				if(Find_Str((char*)usart2_rxbuf,(char *)ch2,200) != 0){
//					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
//					HAL_Delay(1000);
//				}
				memset(usart2_rxbuf,0, 256);
				usart2_rxcounter = 0;
				HAL_Delay(1000);
			}

    /* USER CODE BEGIN 3 */
		}
	}
		else Enter_ErrorMode(4);
	}



void KEY1_Pressed(void)
{
	user_main_debug ("我按下了KEY_1\r\n");
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case KEY_0_Pin:KEY0_Pressed();break;
		case KEY_1_Pin:KEY1_Pressed();break;
		default:break;
	}
}
uint8_t Find_Str(char* dest,char* src,uint16_t retry_nms)
{
	retry_nms/=10;                   //超时时间

	while(strstr(dest,src)==NULL && retry_nms)//等待串口接收完毕或超时退出
	{		
		HAL_Delay(10);
		retry_nms--;
	}
	printf("time:%u\n",retry_nms);

	if(retry_nms) return 1;                       

	return 0; 
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USART2)
	{
		usart2_rxbuf[usart2_rxcounter]=usart2_rxone[0];
		usart2_rxcounter++;
		HAL_UART_Receive_IT(&huart2,usart2_rxone,1);
	}
//	uint16_t count=0;
//	while(count++<usart2_rxcounter)
//		printf("%c",usart2_rxbuf[count]);
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
