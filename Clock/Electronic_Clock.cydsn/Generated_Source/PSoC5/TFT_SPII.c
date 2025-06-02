/*******************************************************************************
* File Name: TFT_SPII.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the SPI Master component.
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

#if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
    volatile uint8 TFT_SPII_txBuffer[TFT_SPII_TX_BUFFER_SIZE];
    volatile uint8 TFT_SPII_txBufferFull;
    volatile uint8 TFT_SPII_txBufferRead;
    volatile uint8 TFT_SPII_txBufferWrite;
#endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

#if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)
    volatile uint8 TFT_SPII_rxBuffer[TFT_SPII_RX_BUFFER_SIZE];
    volatile uint8 TFT_SPII_rxBufferFull;
    volatile uint8 TFT_SPII_rxBufferRead;
    volatile uint8 TFT_SPII_rxBufferWrite;
#endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

uint8 TFT_SPII_initVar = 0u;

volatile uint8 TFT_SPII_swStatusTx;
volatile uint8 TFT_SPII_swStatusRx;


/*******************************************************************************
* Function Name: TFT_SPII_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the
*  interrupt service routine, configuring the bit-counter parameters and
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_Init(void) 
{
    /* Initialize the Bit counter */
    TFT_SPII_COUNTER_PERIOD_REG = TFT_SPII_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != TFT_SPII_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (TFT_SPII_TX_ISR_NUMBER);
        CyIntSetPriority     (TFT_SPII_TX_ISR_NUMBER,  TFT_SPII_TX_ISR_PRIORITY);
        (void) CyIntSetVector(TFT_SPII_TX_ISR_NUMBER, &TFT_SPII_TX_ISR);
    #endif /* (0u != TFT_SPII_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != TFT_SPII_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (TFT_SPII_RX_ISR_NUMBER);
        CyIntSetPriority     (TFT_SPII_RX_ISR_NUMBER,  TFT_SPII_RX_ISR_PRIORITY);
        (void) CyIntSetVector(TFT_SPII_RX_ISR_NUMBER, &TFT_SPII_RX_ISR);
    #endif /* (0u != TFT_SPII_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    TFT_SPII_ClearFIFO();

    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)
        TFT_SPII_rxBufferFull  = 0u;
        TFT_SPII_rxBufferRead  = 0u;
        TFT_SPII_rxBufferWrite = 0u;
    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        TFT_SPII_txBufferFull  = 0u;
        TFT_SPII_txBufferRead  = 0u;
        TFT_SPII_txBufferWrite = 0u;
    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    (void) TFT_SPII_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) TFT_SPII_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    TFT_SPII_TX_STATUS_MASK_REG = TFT_SPII_TX_INIT_INTERRUPTS_MASK;
    TFT_SPII_RX_STATUS_MASK_REG = TFT_SPII_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: TFT_SPII_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void TFT_SPII_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    TFT_SPII_COUNTER_CONTROL_REG |= TFT_SPII_CNTR_ENABLE;
    TFT_SPII_TX_STATUS_ACTL_REG  |= TFT_SPII_INT_ENABLE;
    TFT_SPII_RX_STATUS_ACTL_REG  |= TFT_SPII_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != TFT_SPII_INTERNAL_CLOCK)
        TFT_SPII_IntClock_Enable();
    #endif /* (0u != TFT_SPII_INTERNAL_CLOCK) */

    TFT_SPII_EnableTxInt();
    TFT_SPII_EnableRxInt();
}


/*******************************************************************************
* Function Name: TFT_SPII_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_Start(void) 
{
    if(0u == TFT_SPII_initVar)
    {
        TFT_SPII_Init();
        TFT_SPII_initVar = 1u;
    }

    TFT_SPII_Enable();
}


/*******************************************************************************
* Function Name: TFT_SPII_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void TFT_SPII_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    TFT_SPII_TX_STATUS_ACTL_REG &= ((uint8) ~TFT_SPII_INT_ENABLE);
    TFT_SPII_RX_STATUS_ACTL_REG &= ((uint8) ~TFT_SPII_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != TFT_SPII_INTERNAL_CLOCK)
        TFT_SPII_IntClock_Disable();
    #endif /* (0u != TFT_SPII_INTERNAL_CLOCK) */

    TFT_SPII_DisableTxInt();
    TFT_SPII_DisableRxInt();
}


/*******************************************************************************
* Function Name: TFT_SPII_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_EnableTxInt(void) 
{
    #if(0u != TFT_SPII_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(TFT_SPII_TX_ISR_NUMBER);
    #endif /* (0u != TFT_SPII_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_EnableRxInt(void) 
{
    #if(0u != TFT_SPII_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(TFT_SPII_RX_ISR_NUMBER);
    #endif /* (0u != TFT_SPII_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_DisableTxInt(void) 
{
    #if(0u != TFT_SPII_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(TFT_SPII_TX_ISR_NUMBER);
    #endif /* (0u != TFT_SPII_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_DisableRxInt(void) 
{
    #if(0u != TFT_SPII_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(TFT_SPII_RX_ISR_NUMBER);
    #endif /* (0u != TFT_SPII_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void TFT_SPII_SetTxInterruptMode(uint8 intSrc) 
{
    TFT_SPII_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: TFT_SPII_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void TFT_SPII_SetRxInterruptMode(uint8 intSrc) 
{
    TFT_SPII_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: TFT_SPII_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  TFT_SPII_swStatusTx - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 TFT_SPII_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        TFT_SPII_DisableTxInt();

        tmpStatus = TFT_SPII_GET_STATUS_TX(TFT_SPII_swStatusTx);
        TFT_SPII_swStatusTx = 0u;

        TFT_SPII_EnableTxInt();

    #else

        tmpStatus = TFT_SPII_TX_STATUS_REG;

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: TFT_SPII_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  TFT_SPII_swStatusRx - used to store in software Rx status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 TFT_SPII_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        TFT_SPII_DisableRxInt();

        tmpStatus = TFT_SPII_GET_STATUS_RX(TFT_SPII_swStatusRx);
        TFT_SPII_swStatusRx = 0u;

        TFT_SPII_EnableRxInt();

    #else

        tmpStatus = TFT_SPII_RX_STATUS_REG;

    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: TFT_SPII_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  TFT_SPII_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  TFT_SPII_txBuffer[TFT_SPII_TX_BUFFER_SIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_WriteTxData(uint8 txData) 
{
    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = TFT_SPII_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (TFT_SPII_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == TFT_SPII_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        TFT_SPII_DisableTxInt();

        tempStatus = TFT_SPII_GET_STATUS_TX(TFT_SPII_swStatusTx);
        TFT_SPII_swStatusTx = tempStatus;


        if((TFT_SPII_txBufferRead == TFT_SPII_txBufferWrite) &&
           (0u != (TFT_SPII_swStatusTx & TFT_SPII_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG8(TFT_SPII_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            TFT_SPII_txBufferWrite++;
            if(TFT_SPII_txBufferWrite >= TFT_SPII_TX_BUFFER_SIZE)
            {
                TFT_SPII_txBufferWrite = 0u;
            }

            if(TFT_SPII_txBufferWrite == TFT_SPII_txBufferRead)
            {
                TFT_SPII_txBufferRead++;
                if(TFT_SPII_txBufferRead >= TFT_SPII_TX_BUFFER_SIZE)
                {
                    TFT_SPII_txBufferRead = 0u;
                }
                TFT_SPII_txBufferFull = 1u;
            }

            TFT_SPII_txBuffer[TFT_SPII_txBufferWrite] = txData;

            TFT_SPII_TX_STATUS_MASK_REG |= TFT_SPII_STS_TX_FIFO_NOT_FULL;
        }

        TFT_SPII_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (TFT_SPII_TX_STATUS_REG & TFT_SPII_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG8(TFT_SPII_TXDATA_PTR, txData);

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  TFT_SPII_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  TFT_SPII_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  TFT_SPII_rxBuffer[TFT_SPII_RX_BUFFER_SIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 TFT_SPII_ReadRxData(void) 
{
    uint8 rxData;

    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        TFT_SPII_DisableRxInt();

        if(TFT_SPII_rxBufferRead != TFT_SPII_rxBufferWrite)
        {
            if(0u == TFT_SPII_rxBufferFull)
            {
                TFT_SPII_rxBufferRead++;
                if(TFT_SPII_rxBufferRead >= TFT_SPII_RX_BUFFER_SIZE)
                {
                    TFT_SPII_rxBufferRead = 0u;
                }
            }
            else
            {
                TFT_SPII_rxBufferFull = 0u;
            }
        }

        rxData = TFT_SPII_rxBuffer[TFT_SPII_rxBufferRead];

        TFT_SPII_EnableRxInt();

    #else

        rxData = CY_GET_REG8(TFT_SPII_RXDATA_PTR);

    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: TFT_SPII_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  TFT_SPII_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  TFT_SPII_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 TFT_SPII_GetRxBufferSize(void) 
{
    uint8 size;

    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        TFT_SPII_DisableRxInt();

        if(TFT_SPII_rxBufferRead == TFT_SPII_rxBufferWrite)
        {
            size = 0u;
        }
        else if(TFT_SPII_rxBufferRead < TFT_SPII_rxBufferWrite)
        {
            size = (TFT_SPII_rxBufferWrite - TFT_SPII_rxBufferRead);
        }
        else
        {
            size = (TFT_SPII_RX_BUFFER_SIZE - TFT_SPII_rxBufferRead) + TFT_SPII_rxBufferWrite;
        }

        TFT_SPII_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (TFT_SPII_RX_STATUS_REG & TFT_SPII_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: TFT_SPII_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  TFT_SPII_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  TFT_SPII_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  TFT_SPII_GetTxBufferSize(void) 
{
    uint8 size;

    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        TFT_SPII_DisableTxInt();

        if(TFT_SPII_txBufferRead == TFT_SPII_txBufferWrite)
        {
            size = 0u;
        }
        else if(TFT_SPII_txBufferRead < TFT_SPII_txBufferWrite)
        {
            size = (TFT_SPII_txBufferWrite - TFT_SPII_txBufferRead);
        }
        else
        {
            size = (TFT_SPII_TX_BUFFER_SIZE - TFT_SPII_txBufferRead) + TFT_SPII_txBufferWrite;
        }

        TFT_SPII_EnableTxInt();

    #else

        size = TFT_SPII_TX_STATUS_REG;

        if(0u != (size & TFT_SPII_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & TFT_SPII_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = TFT_SPII_FIFO_SIZE;
        }

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: TFT_SPII_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  TFT_SPII_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(TFT_SPII_RX_STATUS_REG & TFT_SPII_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(TFT_SPII_RXDATA_PTR);
    }

    #if(TFT_SPII_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        TFT_SPII_DisableRxInt();

        TFT_SPII_rxBufferFull  = 0u;
        TFT_SPII_rxBufferRead  = 0u;
        TFT_SPII_rxBufferWrite = 0u;

        TFT_SPII_EnableRxInt();
    #endif /* (TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: TFT_SPII_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  TFT_SPII_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  TFT_SPII_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    TFT_SPII_AUX_CONTROL_DP0_REG |= ((uint8)  TFT_SPII_TX_FIFO_CLR);
    TFT_SPII_AUX_CONTROL_DP0_REG &= ((uint8) ~TFT_SPII_TX_FIFO_CLR);

    #if(TFT_SPII_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        TFT_SPII_AUX_CONTROL_DP1_REG |= ((uint8)  TFT_SPII_TX_FIFO_CLR);
        TFT_SPII_AUX_CONTROL_DP1_REG &= ((uint8) ~TFT_SPII_TX_FIFO_CLR);
    #endif /* (TFT_SPII_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        TFT_SPII_DisableTxInt();

        TFT_SPII_txBufferFull  = 0u;
        TFT_SPII_txBufferRead  = 0u;
        TFT_SPII_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        TFT_SPII_TX_STATUS_MASK_REG &= ((uint8) ~TFT_SPII_STS_TX_FIFO_NOT_FULL);

        TFT_SPII_EnableTxInt();
    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != TFT_SPII_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: TFT_SPII_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void TFT_SPII_TxEnable(void) 
    {
        TFT_SPII_CONTROL_REG |= TFT_SPII_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: TFT_SPII_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void TFT_SPII_TxDisable(void) 
    {
        TFT_SPII_CONTROL_REG &= ((uint8) ~TFT_SPII_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != TFT_SPII_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: TFT_SPII_PutArray
********************************************************************************
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_PutArray(const uint8 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        TFT_SPII_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: TFT_SPII_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void TFT_SPII_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(TFT_SPII_RX_STATUS_REG & TFT_SPII_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(TFT_SPII_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    TFT_SPII_AUX_CONTROL_DP0_REG |= ((uint8)  TFT_SPII_TX_FIFO_CLR);
    TFT_SPII_AUX_CONTROL_DP0_REG &= ((uint8) ~TFT_SPII_TX_FIFO_CLR);

    #if(TFT_SPII_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        TFT_SPII_AUX_CONTROL_DP1_REG |= ((uint8)  TFT_SPII_TX_FIFO_CLR);
        TFT_SPII_AUX_CONTROL_DP1_REG &= ((uint8) ~TFT_SPII_TX_FIFO_CLR);
    #endif /* (TFT_SPII_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: TFT_SPII_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_EnableInt(void) 
{
    TFT_SPII_EnableRxInt();
    TFT_SPII_EnableTxInt();
}


/*******************************************************************************
* Function Name: TFT_SPII_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void TFT_SPII_DisableInt(void) 
{
    TFT_SPII_DisableTxInt();
    TFT_SPII_DisableRxInt();
}


/*******************************************************************************
* Function Name: TFT_SPII_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void TFT_SPII_SetInterruptMode(uint8 intSrc) 
{
    TFT_SPII_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~TFT_SPII_STS_SPI_IDLE));
    TFT_SPII_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: TFT_SPII_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  TFT_SPII_swStatus - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 TFT_SPII_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(TFT_SPII_TX_SOFTWARE_BUF_ENABLED || TFT_SPII_RX_SOFTWARE_BUF_ENABLED)

        TFT_SPII_DisableInt();

        tmpStatus  = TFT_SPII_GET_STATUS_RX(TFT_SPII_swStatusRx);
        tmpStatus |= TFT_SPII_GET_STATUS_TX(TFT_SPII_swStatusTx);
        tmpStatus &= ((uint8) ~TFT_SPII_STS_SPI_IDLE);

        TFT_SPII_swStatusTx = 0u;
        TFT_SPII_swStatusRx = 0u;

        TFT_SPII_EnableInt();

    #else

        tmpStatus  = TFT_SPII_RX_STATUS_REG;
        tmpStatus |= TFT_SPII_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~TFT_SPII_STS_SPI_IDLE);

    #endif /* (TFT_SPII_TX_SOFTWARE_BUF_ENABLED || TFT_SPII_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
