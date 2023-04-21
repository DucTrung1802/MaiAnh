/********************************************************************************************************/
/*
 * @file    IP/Example/main.c
 * @version $Rev:: 4869         $
 * @date    $Date:: 2020-08-05 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
// <<< Use Configuration Wizard in Context Menu >>>

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"

#include "ht32_board.h"

#include "stdlib.h"

#include "string.h"

/** @addtogroup Project_Template Project Template
 * @{
 */

/** @addtogroup IP_Examples IP
 * @{
 */

/** @addtogroup Example
 * @{
 */

/* Settings ------------------------------------------------------------------------------------------------*/
struct Meimei {
  /* Debug */
  char * log_content;
	
	/* Command */
	vu32 command_timer;
	char *command;
	u16 module_buffer_index;
	char *module_buffer;
} Meimei;

/* Private types -------------------------------------------------------------------------------------------*/


/* Private constants ---------------------------------------------------------------------------------------*/
#define LOG_CONTENT_SIZE 100
#define COMMAND_TIMEOUT_MS 500
#define COMMAND_SIZE 100
#define MODULE_BUFFER_SIZE 100


/* Private function prototypes -----------------------------------------------------------------------------*/

/* UART ports */
void UART0_GNSS_Configuration(void);
void UART0_Receive(void);

void USART0_MODULE_Configuration(void);
void USART0_Send_Char(u16 Data);
void USART0_Send(char * input_string);
void USART0_Receive(struct Meimei *self);

void USART1_DEBUG_Configuration(void);
void USART1_Send_Char(u16 Data);
void USART1_Send(char * input_string);
void USART1_Receive(void);

void LED_Init(void);
void Toggle_LED_1(void);
void Toggle_LED_2(void);
void Toggle_LED_3(void);

static void delay_ms(u32 count);
void setup(struct Meimei * self);
void loop(struct Meimei * self);
void sendCommand(struct Meimei * self);
void clearModuleBuffer(struct Meimei *self);

/* Debug */
void writeLog(struct Meimei * self);

/* Private macro -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
struct Meimei meimei_h;
vu32 utick = 0;

/* Global functions ----------------------------------------------------------------------------------------*/

/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
int main(void) {
  setup(&meimei_h);

  while (1) {
    loop(&meimei_h);
  }
}

/********************************************************************************************************/
/*
 * @brief  Main program.
 * @retval None
 ***********************************************************************************************************/
void setup(struct Meimei * self) {
  /* Initialize system tick */
  SysTick_Config(SystemCoreClock / 1000);
	
	/* Initialize LED */
	LED_Init();

  /* Initialize UART ports */
  UART0_GNSS_Configuration();
  USART0_MODULE_Configuration();
  USART1_DEBUG_Configuration();

  /* Initialize Meimei_handler */
  self->log_content = (char * ) malloc(LOG_CONTENT_SIZE * sizeof(char));
  if (!self -> log_content) {
    Toggle_LED_1();
    while (1);
  }
	
  self->command = (char * ) malloc(COMMAND_SIZE * sizeof(char));
  if (!self -> log_content) {
    Toggle_LED_1();
    while (1);
  }
	
  self->module_buffer = (char * ) malloc(MODULE_BUFFER_SIZE * sizeof(char));
  if (!self -> log_content) {
    Toggle_LED_1();
    while (1);
  }

  sprintf(self -> log_content, "Setup successfully!\n");
  writeLog(self);
}

void loop(struct Meimei * self) {
		sprintf(self->command, "AT");
		sendCommand(self);
		delay_ms(500);
}

void sendCommand(struct Meimei * self) {
	
		sprintf(self->log_content, "\n=== SENDING <%s> ===\n", self->command);
		writeLog(self);
	
		clearModuleBuffer(self);
		
		USART0_Send(self->command);
		USART0_Send((char *)"\r\n");

		self->command_timer = utick;
		while(utick - self->command_timer <= COMMAND_TIMEOUT_MS) {
				USART0_Receive(self);
		}
		sprintf(self->log_content, "%s\n", self->module_buffer);
		writeLog(self);
		sprintf(self->log_content, "==========\n");
		writeLog(self);
}

void clearModuleBuffer(struct Meimei *self) {
		for (self->module_buffer_index = 0; self->module_buffer_index < MODULE_BUFFER_SIZE; self->module_buffer_index++) {
				self->module_buffer[self->module_buffer_index] = 0;
		}
		self->module_buffer_index = 0;
}


/* Debug */
void writeLog(struct Meimei * self) {
  USART1_Send(self -> log_content);
}

/*************************************************************************************************************
 * @brief  Configure the UART0 for GNSS
 * @retval None
 ***********************************************************************************************************/
void UART0_GNSS_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PB = 1;
    CKCUClock.Bit.UART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_8, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_UART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_UART0, ENABLE);
  USART_RxCmd(HT_UART0, ENABLE);
}

/*************************************************************************************************************
 * @brief  Configure the USART0
 * @retval None
 ***********************************************************************************************************/
void USART0_MODULE_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_3, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_3, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART0, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART0, ENABLE);
  USART_RxCmd(HT_USART0, ENABLE);
}

/********************************************************************************************************/
/*
 * @brief  Configure the USART1
 * @retval None
 ***********************************************************************************************************/
void USART1_DEBUG_Configuration(void) {
  CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

  {
    /* Enable peripheral clock of AFIO, UxART                                                                 */
    CKCUClock.Bit.AFIO = 1;
    CKCUClock.Bit.PA = 1;
    CKCUClock.Bit.USART1 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_5, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_5, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {
      0
    };
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_USART1, & USART_InitStructure);
  }

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_USART1, ENABLE);
  USART_RxCmd(HT_USART1, ENABLE);
}

/********************************************************************************************************/
/*
 * @brief  UxART0 Tx Send Byte.
 * @param  Data: the data to be transmitted.
 * @retval None
 ***********************************************************************************************************/
void USART0_Send_Char(u16 Data) {
  while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART0, Data);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Send Byte.
 * @param  Data: the data to be transmitted.
 * @retval None
 ***********************************************************************************************************/
void USART1_Send_Char(u16 Data) {
  while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
  USART_SendData(HT_USART1, Data);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Test.
 * @retval None
 ***********************************************************************************************************/
void USART0_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    USART0_Send_Char(input_string[i]);
  }

  /* Send to USART1 what sent to USART0 */
  USART1_Send(input_string);
}

/********************************************************************************************************/
/*
 * @brief  UxART Tx Test.
 * @retval None
 ***********************************************************************************************************/
void USART1_Send(char * input_string) {
  int i;
  /* Send a buffer from UxART to terminal                                                                   */
  for (i = 0; i < strlen(input_string); i++) {
    USART1_Send_Char(input_string[i]);
  }
}

void UART0_Receive(void) {
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_UART0, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_UART0);

    #if 1 // Loop back Rx data to Tx for test
    USART1_Send_Char(uData);
    #endif
  }
}

void USART0_Receive(struct Meimei *self) {
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_USART0, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_USART0);
		self->module_buffer[self->module_buffer_index] = uData;
		self->module_buffer_index++;
  }
	
	/* CONTINUE: Check response OK / ERROR / ... */
}

void USART1_Receive(void) {
  u16 uData;

  /* Waits until the Rx FIFO/DR is not empty then get data from them                                        */
  if (USART_GetFlagStatus(HT_USART1, USART_FLAG_RXDR) == SET) {
    uData = USART_ReceiveData(HT_USART1);

    #if 1 // Loop back Rx data to Tx for test
    USART1_Send_Char(uData);
    #endif
  }
}

void LED_Init() {
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  HT32F_DVB_LEDInit(HT_LED3);
  HT32F_DVB_LEDOn(HT_LED1);
  HT32F_DVB_LEDOff(HT_LED2);
  HT32F_DVB_LEDOn(HT_LED3);
}

void Toggle_LED_1() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED1);
}

void Toggle_LED_2() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED2);
}

void Toggle_LED_3() {
    delay_ms(100);
    HT32F_DVB_LEDToggle(HT_LED3);
}

#if(HT32_LIB_DEBUG == 1)
/********************************************************************************************************/
/*
 * @brief  Report both the error name of the source file and the source line number.
 * @param  filename: pointer to the source file name.
 * @param  uline: error line source number.
 * @retval None
 ***********************************************************************************************************/
void assert_error(u8 * filename, u32 uline) {
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1) {}
}
#endif

/* Private functions ---------------------------------------------------------------------------------------*/
/********************************************************************************************************/
/*
 * @brief  delay function
 * @param  count: delay count for loop
 * @retval None
 ***********************************************************************************************************/
static void delay_ms(u32 ms) {
  uint32_t i, j;
  for (i = 0; i < ms; i++) {
    for (j = 0; j < 11950; j++) {
      __NOP();
    }
  }
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
