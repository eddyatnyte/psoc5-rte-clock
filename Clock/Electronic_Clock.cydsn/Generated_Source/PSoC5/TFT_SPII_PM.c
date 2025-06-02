/*******************************************************************************
* File Name: TFT_SPII_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "TFT_SPII_PVT.h"

static TFT_SPII_BACKUP_STRUCT TFT_SPII_backup =
{
    TFT_SPII_DISABLED,
    TFT_SPII_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: TFT_SPII_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void TFT_SPII_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: TFT_SPII_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void TFT_SPII_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: TFT_SPII_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  TFT_SPII_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_Sleep(void) 
{
    /* Save components enable state */
    TFT_SPII_backup.enableState = ((uint8) TFT_SPII_IS_ENABLED);

    TFT_SPII_Stop();
}


/*******************************************************************************
* Function Name: TFT_SPII_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  TFT_SPII_backup - used when non-retention registers are restored.
*  TFT_SPII_txBufferWrite - modified every function call - resets to
*  zero.
*  TFT_SPII_txBufferRead - modified every function call - resets to
*  zero.
*  TFT_SPII_rxBufferWrite - modified every function call - resets to
*  zero.
*  TFT_SPII_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void TFT_SPII_Wakeup(void) 
{
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

    /* Clear any data from the RX and TX FIFO */
    TFT_SPII_ClearFIFO();

    /* Restore components block enable state */
    if(0u != TFT_SPII_backup.enableState)
    {
        TFT_SPII_Enable();
    }
}


/* [] END OF FILE */
