/*******************************************************************************
* File Name: main.c
*
* Version: 3.0
*
* Description:
*  This code example demonstrates USB HID interface class operation by 
*  implementing a 3-button mouse. When the code is run, the mouse cursor moves 
*  from the right to the left, and vice-versa.
*
*
* Related Document:
*  Device Class Definition for Human Interface Devices (HID)
*  Firmware Specification- Version 1.11
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

#include <project.h>

#define USBFS_DEVICE        (0u)

#define MOUSE_ENDPOINT      (1u)
#define MOUSE_DATA_LEN      (3u)
#define CURSOR_STEP         (5u)
#define CURSOR_STEP_POS     (1u)

/* Mouse packet array: button, X, Y */
uint8 mouseData[MOUSE_DATA_LEN] = {0u, 0u, 0u};
uint8 bSNstring[16u] = {0x0Eu, 0x03u, 'F', 0u, 'W', 0u, 'S', 0u, 'N', 0u, '0', 0u, '1', 0u};


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Wraps the OUT data coming from the host back to the host on a subsequent IN.
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
    uint8 counter = 0u;

    CyGlobalIntEnable;

    /* Set user-defined Serial Number string descriptor. */
    USBFS_SerialNumString(bSNstring);

    /* Start USBFS operation with 5-V operation. */
    USBFS_Start(USBFS_DEVICE, USBFS_5V_OPERATION);

    /* Wait for device to enumerate */
    while (0u == USBFS_GetConfiguration())
    {
    }

    /* Enumeration is done, load mouse endpoint. */
    USBFS_LoadInEP(MOUSE_ENDPOINT, mouseData, MOUSE_DATA_LEN);

    for(;;)
    {
        /* Wait for ACK before loading data. */
        while (0u == USBFS_GetEPAckState(MOUSE_ENDPOINT))
        {
        }

        /* Load endpoint with data after ACK has been received. */
        USBFS_LoadInEP(MOUSE_ENDPOINT, mouseData, MOUSE_DATA_LEN);

        counter++;
        if (counter == 128u)
        {
            /* Start moving mouse to the right. */
            mouseData[CURSOR_STEP_POS] = CURSOR_STEP;
        }
        /* When our counter hits 255. */					
        else if (counter == 255u)
        {
            /* Start moving mouse to left. */
            mouseData[CURSOR_STEP_POS] = (uint8) -(int8) CURSOR_STEP;
        }
        else
        {
            /* Do nothing. */					
        }
    }
}


/* [] END OF FILE */
