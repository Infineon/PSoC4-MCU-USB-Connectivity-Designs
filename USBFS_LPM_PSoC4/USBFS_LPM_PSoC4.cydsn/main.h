/*******************************************************************************
* File Name: main.h
*
* Version: 1.0
*
* Description:
*  This file provides function prototypes, constants and macros for the
*  USBFS Link Power Management (LPM) code example.
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

#if !defined(CY_MAIN_H)
#define CY_MAIN_H

#include <project.h>

    
/*******************************************************************************
*    Function prototypes
*******************************************************************************/

void BulkWrapAround(void);
void LowPowerMode(void);
void HibernateBackUp(void);
void HibernateRestore(void);

/*******************************************************************************
*               Macros
*******************************************************************************/
#define LED_ON              		(0u)
#define LED_OFF             		(1u)

#define TRUE            			(1u)
#define FALSE           			(0u)

#define LED_DEVICE_STATE(x)         LED_RED_Write(x) 
#define LED_DEEP_SLEEP(x)           LED_BLUE_Write(x)
#define LED_HIBERNATE(x)            LED_GREEN_Write(x)

#define RESTORE_DEVICE_ADDRESS(x)   USBFS_CR0_REG = (uint32)(x)| USBFS_CR0_ENABLE

/* USB device number. */
#define USBFS_DEVICE    			(0u)

/* Active endpoints of USB device. */
#define IN_EP_NUM           		(1u)
#define OUT_EP_NUM          		(2u)

#define BUFFER_SIZE        			(64u)

/* Number of 1ms counter ticks before suspend condition is detected. */
#define BESL_BASELINE            	(1u)
#define BESL_DEEP_MODE            	(9u)


#endif /* (CY_MAIN_H) */


/* [] END OF FILE */
