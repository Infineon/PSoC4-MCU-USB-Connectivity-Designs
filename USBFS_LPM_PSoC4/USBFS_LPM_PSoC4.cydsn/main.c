/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This code example demonstrates the USB Link Power Management (LPM) 
*  implementation. The LED on the DVK is turned on when the USB device is 
*  not in the LPM mode and the LED is turned off in the LPM mode. The device 
*  reports two BESL (Best Effort Service Latency (BESL)) values through the
*  BOS descriptor, and depending on the BESL value received from the host, the 
*  device enters either the hibernate mode or deep sleep mode or stays in the
*  active mode.
*  
* Related Document:
*   ECN:  Link Power Management (LPM) - 7/2007
*   Errata for USB 2.0 ECN:  Link Power Management (LPM) - 
*   7/2007 as of October 11, 2011   
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <main.h>


/* Buffer for data transfer from OUT to IN endpoint. */
uint8 buffer[BUFFER_SIZE];
uint8 beslValue;

/* Variables for detection suspend condition on USB bus. */
CY_NOINIT volatile uint8 activeMode;

/* Back up variables - settings to restore after hibernate*/
CY_NOINIT uint8 hibAddressBu;
CY_NOINIT uint8 hibConfigurationBu;
CY_NOINIT uint8 hibInterfaceSetBu[USBFS_MAX_INTERFACES_NUMBER];
CY_NOINIT uint8 hibInterfaceSetLastBu[USBFS_MAX_INTERFACES_NUMBER];
CY_NOINIT T_USBFS_EP_CTL_BLOCK hibUSBFS_EPBu[USBFS_MAX_EP];


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function performs the following actions:
*   1. Starts the USBFS component.
*   2. Waits until the device is enumerated by the host.
*   3. Enables the OUT endpoint to start communication with the host.
*   4. Waits for an LPM request detected.
*   5. The active mode: waits for OUT data coming from the host and sends it back 
*      on a subsequent IN request.
*   6. The low-power mode: when an LPM request BESL is  >= BESL_BASELINE 			!?
*      and < BESL_DEEP_MODE, goes to the deep sleep mode. When an LPM request BESL is >= 
*      BESL_DEEP_MODE, goes to the hibernate mode.
*      The device wakes up when the host drives a resume condition on the bus and 
*      restores components the active mode operation.
*      
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
int main()
{    
    CyGlobalIntEnable;
    
    if (CySysPmGetResetReason() != CY_PM_RESET_REASON_WAKEUP_HIB)
    {
        /* Turn on the red LED - entered active mode from non hibernate reset */
        LED_DEVICE_STATE(LED_ON); 
        LED_DEEP_SLEEP(LED_OFF);  
        LED_HIBERNATE(LED_OFF);  
        
        /* Start USBFS operation with 5V power supply. */
        /* LPM request is ACKed after Start was called. */
        USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);
        
        /* Wait until device is enumerated by host */
        while (0u == USBFS_GetConfiguration())
        {
        } 
        
    }
    else
    {
        /* Restore USBFS to active mode operation. */        
		HibernateRestore();
        
        /* Turn on green LED - entered active mode from hibernate wake-up */
        LED_DEVICE_STATE(LED_OFF); 
        LED_DEEP_SLEEP(LED_OFF);
        LED_HIBERNATE(LED_ON);
    }
    
    /* Enable OUT endpoint to receive data from host */
    USBFS_EnableOutEP(OUT_EP_NUM);

        
    /* Active mode operation after start. */
    activeMode = 1u;
    
    for (;;)
    {
        if (0u != activeMode)
        {
            /* Run USBFS Wraparound Code Example in active mode. */
            BulkWrapAround();
        }
        else
        {
            /* Handle enter/exit from low-power mode. */
            LowPowerMode();
        }
    }
}


/*******************************************************************************
* Function Name: LowPowerMode
********************************************************************************
*
* Summary:
*  This function sets the device power mode based on the BESL value from an LPM request
*   - Active:       BESL < BESL_BASELINE
*   - Deep sleep:   BESL_BASELINE <= BESL < BESL_DEEP_MODE
*   - Hibernate:    BESL >= BESL_DEEP_MODE
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void LowPowerMode(void)
{
    if ((beslValue >= BESL_BASELINE) && (beslValue < BESL_DEEP_MODE))
    {
        /* Turn off all LEDs - enter deep sleep*/
        LED_DEVICE_STATE(LED_OFF); 
        LED_DEEP_SLEEP(LED_OFF);
        LED_HIBERNATE(LED_OFF); 
                
        /* Prepare components before enter low-power mode. */
        USBFS_Suspend();
        CySysPmDeepSleep();       
        
        /* Restore USBFS to active mode operation. */
        USBFS_Resume();
        
        /* Restore communication with host for OUT endpoint. */

        /* Enable OUT endpoint. */
        USBFS_EnableOutEP(OUT_EP_NUM);
    
        /* Active mode operation. */
        activeMode = TRUE;
        
        /* Turn on blue LED - entered active mode from deep sleep wak-eup */
        LED_DEVICE_STATE(LED_OFF); 
        LED_DEEP_SLEEP(LED_ON);
        LED_HIBERNATE(LED_OFF); 
    }
    else if (beslValue >= BESL_DEEP_MODE)
    {
        /* Turn off all LEDs - enter hibernate*/
        LED_DEVICE_STATE(LED_OFF); 
        LED_DEEP_SLEEP(LED_OFF);
        LED_HIBERNATE(LED_OFF); 
                
        /* Prepare components before enter hibernate mode. */
        HibernateBackUp();
        USBFS_Suspend();
        
        CySysPmHibernate();
        
        /* Exit from hibernate is reset. */
    }
    else
    {
        /* Continue active mode operation. */
        activeMode = TRUE;
    }
}


/*******************************************************************************
* Function Name: USBFS_LPM_ISR_EntryCallback
********************************************************************************
*
* Summary:
*  This function is called in the LPM request ISR. It updates the BLES value in the global 
*   besl variable and sets a flag that an LPM request is received 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void USBFS_LPM_ISR_EntryCallback(void)
{
    /* Get BESL value and try to enter low-power mode. */
    beslValue = USBFS_Lpm_GetBeslValue();
    activeMode = FALSE;
}


/*******************************************************************************
* Function Name: USBFS_BUS_RESET_ISR_ExitCallback
********************************************************************************
*
* Summary:
*  This function executes in the Bus reset ISRnd and cleans up the status variables.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void  USBFS_BUS_RESET_ISR_ExitCallback(void)
{
    beslValue = 0u;
    activeMode = FALSE;
    /* Turn on the red LED - entered active mode from non-hibernate reset */
    LED_DEVICE_STATE(LED_ON); 
    LED_DEEP_SLEEP(LED_OFF);  
    LED_HIBERNATE(LED_OFF);
}


/*******************************************************************************
* Function Name: BulkWrapAround
********************************************************************************
*
* Summary:
*  This function executes the USBFS Bulk Wrap Around example project.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void BulkWrapAround(void)
{
    uint16 length;
    
    /* Check if configuration is changed. */
    if (0u != USBFS_IsConfigurationChanged())
    {
        /* Re-enable endpoint when device is configured. */
        if (0u != USBFS_GetConfiguration())
        {
            /* Enable OUT endpoint to receive data from host. */
            USBFS_EnableOutEP(OUT_EP_NUM);
        }
    }

    /* Check if data was received. */
    if (USBFS_OUT_BUFFER_FULL == USBFS_GetEPState(OUT_EP_NUM))
    {
        /* Read number of received data bytes. */
        length = USBFS_GetEPCount(OUT_EP_NUM);

        /* Copy data from OUT endpoint buffer. */
        USBFS_ReadOutEP(OUT_EP_NUM, buffer, length);
        
        /* Check if IN endpoint buffer is empty. */
        if (USBFS_IN_BUFFER_EMPTY == USBFS_GetEPState(IN_EP_NUM))
        {
            /* Copy data into IN endpoint buffer and expose it to be read 
            * by host.
            */
            USBFS_LoadInEP(IN_EP_NUM, buffer, length);    
        }
    }
}


/*******************************************************************************
* Function Name: HibernateBackUp
********************************************************************************
*
* Summary: Back up data structures before hibernate.
*     
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void HibernateBackUp(void)
{
    uint8 i;
    
    hibAddressBu = USBFS_GetDeviceAddress();
    hibConfigurationBu = USBFS_GetConfiguration();
    for (i=0; i<USBFS_MAX_INTERFACES_NUMBER; i++)
    {
        hibInterfaceSetBu[i]=USBFS_interfaceSetting[i];
        hibInterfaceSetLastBu[i] = USBFS_interfaceSettingLast[i];
    }
    
    for (i=0; i<USBFS_MAX_EP; i++)
    {
        hibUSBFS_EPBu[i]=USBFS_EP[i];
    }
    
}


/*******************************************************************************
* Function Name: HibernateRestore
********************************************************************************
*
* Summary: Restores the  registers and data structures after hibernate.		
*  
*      
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void HibernateRestore(void)
{
    uint8 i;
    
    /*Initial restoring of registers*/
    USBFS_Init();
    USBFS_initVar = TRUE;    
    
    
    for (i=0; i<USBFS_MAX_INTERFACES_NUMBER; i++)
    {
        USBFS_interfaceSetting[i] = hibInterfaceSetBu[i];
        USBFS_interfaceSettingLast[i] = hibInterfaceSetLastBu[i];
    }
    for (i=0; i<USBFS_MAX_EP; i++)
    {
        USBFS_EP[i] = hibUSBFS_EPBu[i];
    }
    
    /*Restore configuration registers*/
    USBFS_ConfigReg();
    
    /*Final initialization of component. Pull up Dp*/
    USBFS_InitComponent(USBFS_DEVICE, USBFS_5V_OPERATION);
    
    /*Restore configuration structures*/
    USBFS_configuration = hibConfigurationBu;
    /*Restore device address */
    RESTORE_DEVICE_ADDRESS(hibAddressBu);
}


/* [] END OF FILE */
