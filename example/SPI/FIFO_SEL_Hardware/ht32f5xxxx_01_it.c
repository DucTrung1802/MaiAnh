/*********************************************************************************************************//**
 * @file    SPI/FIFO_SEL_Hardware/ht32f5xxxx_01_it.c
 * @version $Rev:: 4935         $
 * @date    $Date:: 2020-08-26 #$
 * @brief   This file provides all interrupt service routine.
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

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup SPI_Examples SPI
  * @{
  */

/** @addtogroup FIFO_SEL_Hardware
  * @{
  */


/* Private variables ---------------------------------------------------------------------------------------*/
extern vu8 SPI0_Buffer_Rx[];
extern vu8 SPI1_Buffer_Rx[];
extern vu8 SPI0_Rx_Index, SPI1_Rx_Index, tmpflag;

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
 * @brief   This function handles NMI exception.
 * @retval  None
 ************************************************************************************************************/
void NMI_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles Hard Fault exception.
 * @retval  None
 ************************************************************************************************************/
void HardFault_Handler(void)
{
  #if 1

  static vu32 gIsContinue = 0;
  /*--------------------------------------------------------------------------------------------------------*/
  /* For development FW, MCU run into the while loop when the hardfault occurred.                           */
  /* 1. Stack Checking                                                                                      */
  /*    When a hard fault exception occurs, MCU push following register into the stack (main or process     */
  /*    stack). Confirm R13(SP) value in the Register Window and typing it to the Memory Windows, you can   */
  /*    check following register, especially the PC value (indicate the last instruction before hard fault).*/
  /*    SP + 0x00    0x04    0x08    0x0C    0x10    0x14    0x18    0x1C                                   */
  /*           R0      R1      R2      R3     R12      LR      PC    xPSR                                   */
  while (gIsContinue == 0)
  {
  }
  /* 2. Step Out to Find the Clue                                                                           */
  /*    Change the variable "gIsContinue" to any other value than zero in a Local or Watch Window, then     */
  /*    step out the HardFault_Handler to reach the first instruction after the instruction which caused    */
  /*    the hard fault.                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/

  #else

  /*--------------------------------------------------------------------------------------------------------*/
  /* For production FW, you shall consider to reboot the system when hardfault occurred.                    */
  /*--------------------------------------------------------------------------------------------------------*/
  NVIC_SystemReset();

  #endif
}

/*********************************************************************************************************//**
 * @brief   This function handles SVCall exception.
 * @retval  None
 ************************************************************************************************************/
void SVC_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles PendSVC exception.
 * @retval  None
 ************************************************************************************************************/
void PendSV_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles SPI interrupt.
 * @retval  None
 ************************************************************************************************************/
void HTCFG_SPI_MASTER_IRQHandler(void)
{
  bool bIsRxDataReady = FALSE;

  /* Timeout: Clear Timeout Flag                                                                            */
  if (SPI_GetFlagStatus(HTCFG_SPI_MASTER, SPI_FLAG_TOUT))
  {
    SPI_ClearFlag(HTCFG_SPI_MASTER, SPI_FLAG_TOUT);
    bIsRxDataReady = TRUE;
  }

  /* Rx FIFO level is reached                                                                               */
  if (SPI_GetFlagStatus(HTCFG_SPI_MASTER, SPI_FLAG_RXBNE))
  {
    bIsRxDataReady = TRUE;
  }

  /* Rx: Move data from SPI FIFO to buffer                                                                  */
  if (bIsRxDataReady == TRUE)
  {
    u32 uCount = SPI_GetFIFOStatus(HTCFG_SPI_MASTER, SPI_FIFO_RX);

    /* !!! NOTICE !!!
       Turn off the RXBNE interrupt before reading data to prevent the interrupt be triggered again.
       The RXBNE flag will be cleared after reading one data. If new data arrivals at this time and the FIFO
       data count reaches the RX FIFO trigger level (RXFTLS) again, the RXBNE flag will be set and trigger
       the SPI interrupt again.
    */
    SPI_IntConfig(HTCFG_SPI_MASTER, SPI_INT_RXBNE, DISABLE);

    while(uCount--)
    {
      SPI0_Buffer_Rx[SPI0_Rx_Index++] = SPI_ReceiveData(HTCFG_SPI_MASTER);
    }

    SPI_IntConfig(HTCFG_SPI_MASTER, SPI_INT_RXBNE, ENABLE);
  }

  tmpflag++;
}

/*********************************************************************************************************//**
 * @brief   This function handles SPI interrupt.
 * @retval  None
 ************************************************************************************************************/
void HTCFG_SPI_SLAVE_IRQHandler(void)
{
  bool bIsRxDataReady = FALSE;

  /* Timeout: Clear Timeout Flag                                                                            */
  if (SPI_GetFlagStatus(HTCFG_SPI_SLAVE, SPI_FLAG_TOUT))
  {
    SPI_ClearFlag(HTCFG_SPI_SLAVE, SPI_FLAG_TOUT);
    bIsRxDataReady = TRUE;
  }

  /* Rx FIFO level is reached                                                                               */
  if (SPI_GetFlagStatus(HTCFG_SPI_SLAVE, SPI_FLAG_RXBNE))
  {
    bIsRxDataReady = TRUE;
  }

  /* Rx: Move data from SPI FIFO to buffer                                                                  */
  if (bIsRxDataReady == TRUE)
  {
    u32 uCount = SPI_GetFIFOStatus(HTCFG_SPI_SLAVE, SPI_FIFO_RX);

    /* !!! NOTICE !!!
       Turn off the RXBNE interrupt before reading data to prevent the interrupt be triggered again.
       The RXBNE flag will be cleared after reading one data. If new data arrivals at this time and the FIFO
       data count reaches the RX FIFO trigger level (RXFTLS) again, the RXBNE flag will be set and trigger
       the SPI interrupt again.
    */
    SPI_IntConfig(HTCFG_SPI_SLAVE, SPI_INT_RXBNE, DISABLE);

    while(uCount--)
    {
      SPI1_Buffer_Rx[SPI1_Rx_Index++] = SPI_ReceiveData(HTCFG_SPI_SLAVE);
    }

    SPI_IntConfig(HTCFG_SPI_SLAVE, SPI_INT_RXBNE, ENABLE);
  }

  tmpflag++;
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
