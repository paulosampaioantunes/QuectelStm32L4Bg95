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

void InitialConfigBg95(){
    char response[128]; // Buffer temporário para respostas AT

    // Transmitir os comandos AT pela UART2
    sendATCommand("AT+CCID\r\n"); // Obter o ICCID
    HAL_Delay(1000);

    sendATCommand("AT+CIMI\r\n"); // Obter o IMSI
    HAL_Delay(1000);

    sendATCommand("AT+CFUN=0\r\n"); // Configurar o módulo para funcionalidade mínima
    HAL_Delay(1000);

    sendATCommand("AT+QCFG=\"nwscanmode\",0,1\r\n"); // Configurar a pesquisa de todas as RATs
    HAL_Delay(1000);

    sendATCommand("AT+QCFG=\"nwscanseq\",020103,1\r\n"); // Configurar a sequência de pesquisa de RAT
    HAL_Delay(1000);

    sendATCommand("AT+QCFG=\"band\",0,100002000000000F0E189F,10004200000000090E189F\r\n"); // Configurar bandas de frequência
    HAL_Delay(1000);

    sendATCommand("AT+COPS=0\r\n"); // Seleção automática de operadora
    HAL_Delay(1000);

    strcpy(apn, "your_apn");  // Armazenar o APN para uso futuro
    char command[64];
    snprintf(command, sizeof(command), "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
    sendATCommand(command);
    HAL_Delay(1000);

    sendATCommand("AT+CFUN=1\r\n"); // Habilitar funcionalidade completa
    HAL_Delay(1000);

    sendATCommand("AT+CREG=1;+CGREG=1;+CEREG=1\r\n"); // Habilitar códigos de resultado não solicitados para registro de rede
    HAL_Delay(1000);

    sendATCommand("AT+COPS?\r\n"); // Verificar a seleção de operadora
    HAL_Delay(1000);

    sendATCommand("AT+QCSQ\r\n"); // Verificar a tecnologia ativa e a força do sinal
    HAL_Delay(1000);

    sendATCommand("AT+CREG?;+CEREG?;+CGREG?\r\n"); // Verificar o estado de registro na rede
    HAL_Delay(1000);

    sendATCommand("AT+CGATT=1\r\n"); // Abrir um contexto de dados
    HAL_Delay(1000);

    sendATCommand("AT+CGPADDR\r\n"); // Verificar o endereço IP alocado
    HAL_Delay(1000);

    // Receber o IP de resposta e armazenar na variável global
    receiveResponse(response, sizeof(response));
    sscanf(response, "+CGPADDR: 1,\"%15[^\"]", ip); // Extrair o IP da resposta e salvar
    HAL_Delay(1000);

    sendATCommand("AT+QPING=1,\"192.168.109.2\"\r\n"); // Realizar um teste de ping
    HAL_Delay(1000);
}
void ConfigPdpContext(){
    char command[64];
    char response[128];

    // Enviar o comando para ativar o contexto PDP
    sendATCommand("AT+QIACT=1\r\n");
    HAL_Delay(1000);

    // Verificar se a ativação foi bem-sucedida
    sendATCommand("AT+QIACT?\r\n");
    HAL_Delay(1000);

    // Receber a resposta e verificar se contém o IP (verificação básica)
    receiveResponse(response, sizeof(response));
    if (strstr(response, ip)) {
        // Ativação bem-sucedida, o IP foi alocado corretamente
        snprintf(command, sizeof(command), "PDP context ativado com sucesso. APN: %s, IP: %s\r\n", apn, ip);
        sendATCommand(command); // Transmitir a mensagem de sucesso pela UART
    } else {
        // Erro ao ativar o contexto PDP
        sendATCommand("Erro ao ativar o contexto PDP\r\n");
    }
}
void ConfigMqttContext() {
    // Enviar comandos de configuração inicial do MQTT via UART
    sendATCommand("AT+QMTCFG=\"version\",0,4\r\n"); // Definir a versão do MQTT como 3.1.1 (versão 4)
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n"); // Associar o contexto PDP ao cliente MQTT
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"ssl\",0,1,0\r\n"); // Habilitar SSL para o contexto MQTT
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"keepalive\",0,0\r\n"); // Configurar o intervalo de keepalive para 0 (sem limite)
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"session\",0,1\r\n"); // Configurar para manter a sessão ativa após desconexão
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"timeout\",0,90,0,1\r\n"); // Definir o timeout de 90 segundos
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"will\",0,1,0,1,\"/test/will\",\"Client disconnected unexpectedly\"\r\n"); // Configurar a mensagem de "will" (última vontade)
    HAL_Delay(1000);

    sendATCommand("AT+QMTCFG=\"recv/mode\",0,0,1\r\n"); // Configurar o modo de recebimento
    HAL_Delay(1000);

}
void ActivePdp() {
    // Verifica se a APN está configurada corretamente
    sendATCommand("AT+CGDCONT?\r\n");
    HAL_Delay(1000);

    // Ativa o contexto PDP (com o CID 1)
    sendATCommand("AT+CGACT=1,1\r\n");
    HAL_Delay(1000);

    // Verifica se um endereço IP foi atribuído
    sendATCommand("AT+CGPADDR=1\r\n");
    HAL_Delay(1000);

    // Verifica se a APN e a rede estão devidamente configuradas
    sendATCommand("AT+COPS?\r\n");
    HAL_Delay(1000);

    // Tenta desconectar um canal MQTT ocupado e tenta se conectar a outro
    for (int channel = 1; channel <= 5; channel++) {
        // Desconecta o canal ocupado
        char command[30];
        sprintf(command, "AT+QMTDISC=%d\r\n", channel);
        sendATCommand(command);
        HAL_Delay(1000);
    }
}
void MqttConnectAndSubscribe() {
    // Abrir conexão com o broker MQTT (usando o canal 3)
    sendATCommand("AT+QMTOPEN=3,\"3.228.101.77\",1883\r\n"); // ou usar o IP "172.31.82.252"
    HAL_Delay(2000); // Aguardar resposta do comando
    // Conectar ao broker MQTT com ID "teste" e credenciais "pixtest"
    sendATCommand("AT+QMTCONN=3,\"teste\",\"pixtest\",\"pixtest\"\r\n");
    HAL_Delay(2000); // Aguardar resposta do comando
    // Assinar o tópico "test" no nível QoS 0 para receber dados
    sendATCommand("AT+QMTSUB=3,1,\"test\",0\r\n");
    HAL_Delay(1000); // Aguardar resposta do comando

}
void MqttConfigBeforeConnection() {
    // Verificar informações do módulo
    sendATCommand("ATI\r\n");
    HAL_Delay(1000);

    // Verificar o estado de operação do dispositivo
    sendATCommand("AT+CFUN?\r\n");
    HAL_Delay(500);

    // Verificar o modo de varredura de rede
    sendATCommand("AT+QCFG=\"nwscanmode\"\r\n");
    HAL_Delay(500);

    // Verificar a sequência de varredura de rede
    sendATCommand("AT+QCFG=\"nwscanseq\"\r\n");
    HAL_Delay(500);

    // Verificar o modo IoT
    sendATCommand("AT+QCFG=\"iotopmode\"\r\n");
    HAL_Delay(500);

    // Verificar o domínio de serviço
    sendATCommand("AT+QCFG=\"servicedomain\"\r\n");
    HAL_Delay(500);

    // Configurar a porta para URC (USB Modem)
    sendATCommand("AT+QURCCFG=\"urcport\",\"usbmodem\"\r\n");
    HAL_Delay(500);

    // Verificar o status de registro na rede
    sendATCommand("AT+CREG?\r\n");
    HAL_Delay(500);

    // Verificar o status de registro GPRS
    sendATCommand("AT+CGREG?\r\n");
    HAL_Delay(500);

    // Verificar a operadora atual
    sendATCommand("AT+COPS?\r\n");
    HAL_Delay(500);

    // Verificar a qualidade do sinal
    sendATCommand("AT+CSQ\r\n");
    HAL_Delay(500);

    // Verificar a qualidade do sinal detalhada
    sendATCommand("AT+QCSQ\r\n");
    HAL_Delay(500);

    // Obter informações de rede
    sendATCommand("AT+QNWINFO\r\n");
    HAL_Delay(500);

    // Verificar se o dispositivo está anexado à rede
    sendATCommand("AT+CGATT?\r\n");
    HAL_Delay(500);

    // Verificar o status de ativação de contexto PDP
    sendATCommand("AT+CGACT?\r\n");
    HAL_Delay(500);

    // Verificar a configuração de contexto PDP
    sendATCommand("AT+CGDCONT?\r\n");
    HAL_Delay(500);

    // Configurar o contexto PDP para o canal MQTT (fazer para todos os canais 0-5)
    for (int i = 0; i <= 5; i++) {
        char command[50];
        snprintf(command, sizeof(command), "AT+QMTCFG=\"pdpcid\",%d,1\r\n", i);
        sendATCommand(command);
        HAL_Delay(500);
    }

    // Configurar o keepalive MQTT para 3600 segundos (fazer para todos os canais 0-5)
    for (int i = 0; i <= 5; i++) {
        char command[50];
        snprintf(command, sizeof(command), "AT+QMTCFG=\"keepalive\",%d,3600\r\n", i);
        sendATCommand(command);
        HAL_Delay(500);
    }
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
