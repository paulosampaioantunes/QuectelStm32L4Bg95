/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define APN_MAX_LENGTH 32
#define IP_MAX_LENGTH  16
char apn[APN_MAX_LENGTH];
char ip[IP_MAX_LENGTH];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void sendATCommand(char* command);
void InitialConfigBg95();
void receiveResponse(char* buffer, uint16_t bufferSize);
void ConfigPdpContext();
void ConfigMqttContext();
void ActivePdp();
void MqttConnectAndSubscribe();
void MqttConfigBeforeConnection();



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
  MX_RTC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  InitialConfigBg95();
	  ConfigPdpContext();
	  ActivePdp();
	  ConfigMqttContext();
	  MqttConfigBeforeConnection();
	  MqttConnectAndSubscribe();

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void sendATCommand(char* command) {
    HAL_UART_Transmit(&huart2, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
}
void receiveResponse(char* buffer, uint16_t bufferSize) {
    HAL_UART_Receive(&huart2, (uint8_t*)buffer, bufferSize, HAL_MAX_DELAY);
}

void InitialConfigBg95() {
    char response[128];   // Buffer temporário para respostas AT
    char ip[16];          // Buffer para armazenar o endereço IP
    char command[128];    // Buffer para os comandos AT

    // Transmitir os comandos AT pela UART2

    // Comando AT+CCID
    sendATCommand("AT+CCID\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CCID: %s\n", response);

    // Comando AT+CIMI
    sendATCommand("AT+CIMI\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CIMI: %s\n", response);

    // Comando AT+CFUN=0
    sendATCommand("AT+CFUN=0\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CFUN=0: %s\n", response);

    // Comando AT+QCFG="nwscanmode",0,1
    sendATCommand("AT+QCFG=\"nwscanmode\",0,1\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+QCFG=\"nwscanmode\": %s\n", response);

    // Comando AT+QCFG="nwscanseq",020103,1
    sendATCommand("AT+QCFG=\"nwscanseq\",020103,1\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+QCFG=\"nwscanseq\": %s\n", response);

    // Comando AT+QCFG="band",0,0
    sendATCommand("AT+QCFG=\"band\",0,0\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+QCFG=\"band\": %s\n", response);

    // Comando AT+COPS=0
    sendATCommand("AT+COPS=0\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+COPS=0: %s\n", response);

    // Configurar o APN, username e password para a Vivo
    sendATCommand("AT+CGDCONT=1,\"IP\",\"inlog.vivo.com.br\",\"datatem\",\"datatem\"\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CGDCONT: %s\n", response);

    // Comando AT+CFUN=1
    sendATCommand("AT+CFUN=1\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CFUN=1: %s\n", response);

    // Comando AT+CREG=1;+CGREG=1;+CEREG=1
    sendATCommand("AT+CREG=1;+CGREG=1;+CEREG=1\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CREG=1;+CGREG=1;+CEREG=1: %s\n", response);

    // Comando AT+COPS?
    sendATCommand("AT+COPS?\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+COPS?: %s\n", response);

    // Comando AT+QCSQ
    sendATCommand("AT+QCSQ\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+QCSQ: %s\n", response);

    // Comando AT+CREG?;+CEREG?;+CGREG?
    sendATCommand("AT+CREG?;+CEREG?;+CGREG?\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CREG?;+CEREG?;+CGREG?: %s\n", response);

    // Comando AT+CGATT=1 (Conectar à rede)
    sendATCommand("AT+CGATT=1\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CGATT=1: %s\n", response);

    // Verificar o endereço IP alocado
    sendATCommand("AT+CGPADDR\r\n");
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+CGPADDR: %s\n", response);

    // Extrair o endereço IP da resposta
    if (sscanf(response, "+CGPADDR: 1,\"%15[^\"]", ip) == 1) {
        printf("Endereço IP alocado: %s\n", ip); // Exibir o IP
    } else {
        printf("Falha ao obter o endereço IP.\n");
    }

    // Realizar um teste de ping com o IP obtido
    snprintf(command, sizeof(command), "AT+QPING=1,\"%s\"\r\n", ip);
    sendATCommand(command);
    HAL_Delay(10);
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    printf("Resposta AT+QPING: %s\n", response);
}


void ConfigPdpContext() {
    char command[128];
    char response[256];

    // Configurar APN com o contexto CID 1, com login e senha
    snprintf(command, sizeof(command), "AT+CGDCONT=1,\"IP\",\"inlog.vivo.com.br\",\"\",0,0\r\n");
    sendATCommand(command);

    // Ativar o contexto PDP
    sendATCommand("AT+CGACT=1,1\r\n");

    // Verificar se o contexto PDP está ativo
    sendATCommand("AT+CGPADDR=1\r\n");

    // Receber a resposta e verificar se contém o IP
    HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    if (strstr(response, "0.0.0.0") == NULL) {
        // Contexto PDP ativado com sucesso, IP foi alocado corretamente
        snprintf(command, sizeof(command), "PDP context ativado com sucesso. APN: inlog.vivo.com.br, IP: %s\r\n", response);
        sendATCommand(command); // Transmitir a mensagem de sucesso pela UART
    } else {
        // Erro ao ativar o contexto PDP
        sendATCommand("Erro ao ativar o contexto PDP\r\n");
    }
}


void ConfigMqttContext() {
    // Configurar a versão MQTT como 3.1.1
    sendATCommand("AT+QMTCFG=\"version\",0,4\r\n");
    // Associar o contexto PDP ao cliente MQTT
    sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
    // Habilitar SSL para o cliente MQTT
    sendATCommand("AT+QMTCFG=\"ssl\",0,1,0\r\n");
    // Configurar keepalive para 3600 segundos
    sendATCommand("AT+QMTCFG=\"keepalive\",0,3600\r\n");
    // Configurar o "Will Message" do MQTT
    sendATCommand("AT+QMTCFG=\"will\",0,1,0,1,\"/test/will\",\"Client disconnected unexpectedly\"\r\n");
}

void ActivePdp() {
    // Verificar se o APN está configurado corretamente
    sendATCommand("AT+CGDCONT?\r\n");
    // Ativar o contexto PDP
    sendATCommand("AT+CGACT=1,1\r\n");
    // Verificar se um endereço IP foi atribuído
    sendATCommand("AT+CGPADDR=1\r\n");
}
void MqttConnectAndSubscribe() {
    // Conectar ao broker MQTT
    sendATCommand("AT+QMTOPEN=0,\"broker.mqttdashboard.com\",1883\r\n");
    // Conectar ao broker MQTT com ID e credenciais
    sendATCommand("AT+QMTCONN=0,\"clienteID\",\"usuario\",\"senha\"\r\n");
    // Inscrever-se no tópico desejado
    sendATCommand("AT+QMTSUB=0,\"topico/assinar\",1\r\n");
}

void MqttConfigBeforeConnection() {

    // Verificar se o dispositivo está registrado na rede
    sendATCommand("AT+CREG?\r\n");
    // Verificar o status do contexto PDP
    sendATCommand("AT+CGACT?\r\n");
    // Verificar a qualidade do sinal
    sendATCommand("AT+CSQ\r\n");
    // Configurar o cntexto PDP para o canal MQTT
    sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
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
