/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This example project demonstrates the ability of the USBFS component to 
*  detect a suspend condition on the USB bus and resume its operation when 
*  a resume condition is detected and the USB bus activity restored.
*  The PSoC operates in the active power mode before the host decides to 
*  suspend the USB device. The PSoC mode is changed to low power as soon as 
*  a suspend condition is detected to consume less power. The USB device 
*  maximum suspend current should  not exceed 500uA to be compliant with 
*  the USB specification. When the host decides to wake up the USB device 
*  it drives a resume condition on the bus then PSoC wakes up and changes 
*  its power mode to active.
*  The LED is used to indicate the USB device state as well as the PSoC power 
*  mode. The LED is on when the USB bus is active and PSoC is in the active 
*  mode. The LED is off after a suspend condition is detected and PSoC is in 
*  the low-power mode.
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

/* USB device number. */
#define USBFS_DEVICE    (0u)

/* Active endpoints of USB device. */
#define IN_EP_NUM       (1u)
#define OUT_EP_NUM      (2u)

/* Buffer for data transfer from OUT to IN endpoint. */
#define BUFFER_SIZE     (64u)
uint8 buffer[BUFFER_SIZE];

/* Variables for detection suspend condition on USB bus. */
uint8 usbIdleCounter = 0u;
uint8 usbSuspend = 0u;

/* Number of 1-ms counter ticks before suspend condition is detected. */
#define SUSPEND_COUNT   (3u)


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function performs the following actions:
*   1. Starts the USBFS component.
*   2. Waits until the device is enumerated by the host.
*   3. Enables the OUT endpoint to start communication with the host.
*   4. Starts a 1-ms tick timer to check the USB activity and detect a suspend 
*      condition on the USB bus.
*   5. The LED is used to indicate the USB device state as well as PSoC state 
*      power state. The LED is on when the USB bus is active and PSoC is in the
*      active state. The LED is off after a suspend condition is detected and 
*      PSoC is in the low-power state.
*   6. While PSoC is in the active power state, the USB device loops back 
*      the data sent to the OUT endpoint.
*   7. When a USB suspend condition is detected, PSoC enters the low-power mode
*      (DeepSleep for PSoC 4200L or Sleep for PSoC 3/PSoC 5LP). A wakeup occurs
*      when the host drives a resume condition on the USB bus and PSoC returns 
*      to the active mode task execution.
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

    /* Start USBFS operation with 5V power supply. */
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);

    /* Wait until device is enumerated by host. It w */
    while (0u == USBFS_GetConfiguration())
    {
    }

    /* Enable OUT endpoint to receive data from host. */
    USBFS_EnableOutEP(OUT_EP_NUM);

    /* Start timer with period of 1ms to track USB activity. SOFs packets
    * start coming after device has been enumerated.
    */
    timerIsr_StartEx(&TimerIsr);
    Timer_Start();

    /* Indicate that device is in active mode. */
    TURN_ON_LED;

    for(;;)
    {
        /* Execute USBFS Wraparound Code Example in active mode. */
        BulkWrapAround();

        /* Check if suspend condition is detected on bus. */
        if (0u != usbSuspend)
        {
            /* Reset suspend detect variables. */
            usbSuspend = 0u;
            usbIdleCounter = 0u;

            /* Indicate that device goes into low-power mode soon. */
            TURN_OFF_LED;

            /* Prepare components before entering low-power mode. */
            Timer_Sleep();
            USBFS_Suspend();

            /* Enter low-power mode: DeepSleep for PSoC 4 or Sleep for PSoC 3/
            * PSoC 5LP. The device wakes up when the host drives resume on the bus. 
            * The wakeup source is PICU - USB Dp pin falling edge.
            */
        #if (CY_PSOC4)
            CySysPmDeepSleep();
        #else
            CyPmSaveClocks();
            /* Specify wakeup source explicitly. */
            CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_PICU);
            CyPmRestoreClocks();
        #endif /* (CY_PSOC4) */

            /* Restore USBFS to active mode operation. */
            USBFS_Resume();
            Timer_Wakeup();

            /* Restore communication with host for OUT endpoint. */
            /* Data left into IN endpoint buffer is lost. */

            /* Enable OUT endpoint to receive data from host. */
            USBFS_EnableOutEP(OUT_EP_NUM);

            /* Indicate that device is in active mode. */
            TURN_ON_LED;
        }
    }
}


/*******************************************************************************
* Function Name: TimerIsr
********************************************************************************
*
* Summary:
*  This Interrupt Service Routine checks the activity on the USB bus with a period
*  of 1ms. If the bus is idle for more than 3ms, the USB suspend condition is
*  detected.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
CY_ISR(TimerIsr)
{
    /* Check if there has been activity on USB bus since last timer tick. */
    if (0u != USBFS_CheckActivity())
    {
        usbIdleCounter = 0u;
    }
    else
    {
        /* Check for suspend condition on USB bus. */
        if (usbIdleCounter < SUSPEND_COUNT)
        {
            /* Counter idle time before detect suspend condition. */
            ++usbIdleCounter;
        }
        else
        {
            /* Suspend condition on USB bus is detected. Request device to
            * enter low-power mode.
            */
            usbSuspend = 1u;
        }
    }
}


/*******************************************************************************
* Function Name: BulkWrapAround
********************************************************************************
*
* Summary:
*  This function executes the USBFS Bulk Wraparound code example.
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


/* [] END OF FILE */
