/*******************************************************************************
* File Name: TFT_SPII_INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "TFT_SPII_PVT.h"


/* User code required at start of ISR */
/* `#START TFT_SPII_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: TFT_SPII_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  TFT_SPII_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  TFT_SPII_txBuffer[TFT_SPII_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(TFT_SPII_TX_ISR)
{
    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef TFT_SPII_TX_ISR_ENTRY_CALLBACK
        TFT_SPII_TX_ISR_EntryCallback();
    #endif /* TFT_SPII_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START TFT_SPII_TX_ISR_START` */

    /* `#END` */
    
    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(TFT_SPII_txBufferRead != TFT_SPII_txBufferWrite)
        {
            tmpStatus = TFT_SPII_GET_STATUS_TX(TFT_SPII_swStatusTx);
            TFT_SPII_swStatusTx = tmpStatus;

            if(0u != (TFT_SPII_swStatusTx & TFT_SPII_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == TFT_SPII_txBufferFull)
                {
                   TFT_SPII_txBufferRead++;

                    if(TFT_SPII_txBufferRead >= TFT_SPII_TX_BUFFER_SIZE)
                    {
                        TFT_SPII_txBufferRead = 0u;
                    }
                }
                else
                {
                    TFT_SPII_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(TFT_SPII_TXDATA_PTR, 
                                             TFT_SPII_txBuffer[TFT_SPII_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(TFT_SPII_txBufferRead == TFT_SPII_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            TFT_SPII_TX_STATUS_MASK_REG &= ((uint8) ~TFT_SPII_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START TFT_SPII_TX_ISR_END` */

    /* `#END` */
    
    #ifdef TFT_SPII_TX_ISR_EXIT_CALLBACK
        TFT_SPII_TX_ISR_ExitCallback();
    #endif /* TFT_SPII_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: TFT_SPII_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  TFT_SPII_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  TFT_SPII_rxBuffer[TFT_SPII_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(TFT_SPII_RX_ISR)
{
    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef TFT_SPII_RX_ISR_ENTRY_CALLBACK
        TFT_SPII_RX_ISR_EntryCallback();
    #endif /* TFT_SPII_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START TFT_SPII_RX_ISR_START` */

    /* `#END` */
    
    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = TFT_SPII_GET_STATUS_RX(TFT_SPII_swStatusRx);
        TFT_SPII_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (TFT_SPII_swStatusRx & TFT_SPII_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(TFT_SPII_RXDATA_PTR);

            /* Set next pointer. */
            TFT_SPII_rxBufferWrite++;
            if(TFT_SPII_rxBufferWrite >= TFT_SPII_RX_BUFFER_SIZE)
            {
                TFT_SPII_rxBufferWrite = 0u;
            }

            if(TFT_SPII_rxBufferWrite == TFT_SPII_rxBufferRead)
            {
                TFT_SPII_rxBufferRead++;
                if(TFT_SPII_rxBufferRead >= TFT_SPII_RX_BUFFER_SIZE)
                {
                    TFT_SPII_rxBufferRead = 0u;
                }

                TFT_SPII_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            TFT_SPII_rxBuffer[TFT_SPII_rxBufferWrite] = rxData;

            tmpStatus = TFT_SPII_GET_STATUS_RX(TFT_SPII_swStatusRx);
            TFT_SPII_swStatusRx = tmpStatus;
        }

    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START TFT_SPII_RX_ISR_END` */

    /* `#END` */
    
    #ifdef TFT_SPII_RX_ISR_EXIT_CALLBACK
        TFT_SPII_RX_ISR_ExitCallback();
    #endif /* TFT_SPII_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
