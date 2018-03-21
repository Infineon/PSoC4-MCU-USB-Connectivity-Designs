/*******************************************************************************
* File Name: main.c
*
* Version: 3.0
*
* Description:
*  This example project demonstrates the basic operation of the Bootloader and 
*  Bootloadable components when the communication interface is a USB.
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


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function performs the following actions:
*   1. Indicates that the application is running by turning on the LED.
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
    /* Indicates that the application is running. The bootloader passed control to
    * the application.
    */
#if (CY_PSOC4)
    RGB_LED_ON_GREEN;
#else
    TURN_ON_LED3;
#endif /* (CY_PSOC4) */

    /* Application does not execute any other actions. */
    for(;;)
    {
    }
}


/* [] END OF FILE */
