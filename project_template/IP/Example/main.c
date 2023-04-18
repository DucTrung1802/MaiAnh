/*********************************************************************************************************//**
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

#include "string.h"

#define USART_FLAG_TXE ((u16)0x0080)
#define USART_FLAG_RXNE ((uint16_t)0x0020)

#define HTCFG_UART_PORT 									HT_USART0
#define HTCFG_TX_PDMA_CH 									PDMA_USART0_TX
#define HTCFG_RX_PDMA_CH 									PDMA_USART0_RX
#define HTCFG_PDMA_IRQ 										PDMACH0_1_IRQn
#define HTCFG_PDMA_IRQHandler             (PDMA_CH0_1_IRQHandler)
#define HTCFG_UART_RX_GPIO_CLK 						PA
#define HTCFG_UART_IPN										USART0
#define HTCFG_UART_RX_GPIO_PORT 					HT_GPIOA
#define HTCFG_UART_RX_GPIO_PIN  					GPIO_PIN_3
#define HTCFG_UART_TX_GPIO_ID							GPIO_PA
#define HTCFG_UART_TX_AFIO_PIN						AFIO_PIN_2
#define HTCFG_UART_RX_GPIO_ID							GPIO_PA
#define HTCFG_UART_RX_AFIO_PIN						AFIO_PIN_3
#define HTCFG_PDMA_CURRENT_TRANSFER_SIZE  (HT_PDMA->PDMACH0.CTSR >> 16)


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
#define RX_BUFFER_SIZE                            (64)
#define RX_FULL_CHECK                             (0)

/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

void GNSS_UART0_Configuration(void);

void UxART0_TxSend(u16 Data);
void UxART0_Tx(char* input_string);

void UxART1_Configuration(void);
void UxART1_TxSend(u16 Data);
void UxART1_Tx(char* input_string);

void LED_Init(void);
void LED_Toggle(void);

static void __Delay(u32 count);

/* Private macro -------------------------------------------------------------------------------------------*/


/* Global variables ----------------------------------------------------------------------------------------*/


/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
    LED_Init();
    LED_Toggle();
    UxART0_Configuration();
    UxART1_Configuration();

    UxART0_Tx((char*)"AT\r\n");
    UxART1_Tx((char*)"AT\r\n");

    while (1)
    {

    }
}

/*************************************************************************************************************
  * @brief  Configure the UART0 for GNSS
  * @retval None
  ***********************************************************************************************************/
void GNSS_UART0_Configuration(void){
		CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

	{/* Enable peripheral clock of AFIO, UxART                                                                 */
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
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HT_UART0, &USART_InitStructure);
  }
	
	/* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HT_UART0, ENABLE);
  USART_RxCmd(HT_UART0, ENABLE);
}


/*************************************************************************************************************
  * @brief  Configure the UxART
  * @retval None
  ***********************************************************************************************************/
void USART0_Configuration(void)
{
    CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

    {   /* Enable peripheral clock of AFIO, UxART                                                                 */
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
        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
        USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
        USART_InitStructure.USART_Parity = USART_PARITY_NO;
        USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
        USART_Init(HTCFG_UART_PORT, &USART_InitStructure);
    }

#if 0 // Enable UxART TX DMA when Start Tx
    /* Enable UxART TX trigger DMA                                                                            */
    USART_PDMACmd(HTCFG_UART_PORT, USART_PDMAREQ_TX, ENABLE);
#endif

    /* Enable UxART RX trigger DMA                                                                            */
    USART_PDMACmd(HTCFG_UART_PORT, USART_PDMAREQ_RX, ENABLE);

    /* Enable UxART Tx and Rx function                                                                        */
    USART_TxCmd(HTCFG_UART_PORT, ENABLE);
    USART_RxCmd(HTCFG_UART_PORT, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  Configure the UART1.
  * @retval None
  ***********************************************************************************************************/
void UxART1_Configuration(void)
{
    CKCU_PeripClockConfig_TypeDef CKCUClock; // Set all the fields to zero, which means that no peripheral clocks are enabled by default.

    {   /* Enable peripheral clock of AFIO, UxART                                                                 */
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
        USART_InitTypeDef USART_InitStructure = {0};
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
        USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
        USART_InitStructure.USART_Parity = USART_PARITY_NO;
        USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
        USART_Init(HT_USART1, &USART_InitStructure);
    }

    /* Enable UxART Tx and Rx function                                                                        */
    USART_TxCmd(HT_USART1, ENABLE);
    USART_RxCmd(HT_USART1, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  UxART0 Tx Send Byte.
  * @param  Data: the data to be transmitted.
  * @retval None
  ***********************************************************************************************************/
void UxART0_TxSend(u16 Data)
{
    while (USART_GetFlagStatus(HT_USART0, USART_FLAG_TXC) == RESET);
    USART_SendData(HT_USART0, Data);
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Send Byte.
  * @param  Data: the data to be transmitted.
  * @retval None
  ***********************************************************************************************************/
void UxART1_TxSend(u16 Data)
{
    while (USART_GetFlagStatus(HT_USART1, USART_FLAG_TXC) == RESET);
    USART_SendData(HT_USART1, Data);
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Test.
  * @retval None
  ***********************************************************************************************************/
void UxART0_Tx(char* input_string)
{
    int i;
    /* Send a buffer from UxART to terminal                                                                   */
    for (i = 0; i < strlen(input_string); i++)
    {
        UxART0_TxSend(input_string[i]);
    }
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Test.
  * @retval None
  ***********************************************************************************************************/
void UxART1_Tx(char* input_string)
{
    int i;
    /* Send a buffer from UxART to terminal                                                                   */
    for (i = 0; i < strlen(input_string); i++)
    {
        UxART1_TxSend(input_string[i]);
    }
}

void LED_Init()
{
    HT32F_DVB_LEDInit(HT_LED1);
    HT32F_DVB_LEDInit(HT_LED2);
    HT32F_DVB_LEDInit(HT_LED3);
    HT32F_DVB_LEDOn(HT_LED1);
    HT32F_DVB_LEDOff(HT_LED2);
    HT32F_DVB_LEDOn(HT_LED3);
}

void LED_Toggle()
{
    s32 input;
    for (input = 0; input < 10; input++)
    {
        __Delay(2000000);
        HT32F_DVB_LEDToggle(HT_LED1);
        HT32F_DVB_LEDToggle(HT_LED2);
        HT32F_DVB_LEDToggle(HT_LED3);
    }
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
    /*
       This function is called by IP library that the invalid parameters has been passed to the library API.
       Debug message can be added here.
       Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
    */

    while (1)
    {
    }
}
#endif

/* Private functions ---------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  delay function
  * @param  count: delay count for loop
  * @retval None
  ***********************************************************************************************************/
static void __Delay(u32 count)
{
    while (count--)
    {
        __NOP(); // Prevent delay loop be optimized
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
