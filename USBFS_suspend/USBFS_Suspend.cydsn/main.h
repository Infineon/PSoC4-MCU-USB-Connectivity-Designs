/*******************************************************************************
* File Name: main.h
*
* Version: 1.0
*
* Description:
*  This file provides function prototypes, constants and macros for the
*  USBFS Suspend example project.
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


/***************************************
*    Function prototypes
****************************************/

void BulkWrapAround(void);
CY_ISR_PROTO(TimerIsr);


/***************************************
*               Macros
****************************************/

#if (CY_PSOC4)
/* PSoC4: RGB LED is active low on kit. */
    
/* Turn on LED */
#define TURN_ON_LED     RGB_LED_ON_RED
#define TURN_OFF_LED    RGB_LED_OFF

/* Set LED RED color */
#define RGB_LED_ON_RED  \
                do{     \
                    LED_RED_Write  (0u); \
                    LED_GREEN_Write(1u); \
                    LED_BLUE_Write (1u); \
                }while(0)

/* Set LED GREEN color */
#define RGB_LED_ON_GREEN \
                do{      \
                    LED_RED_Write  (1u); \
                    LED_GREEN_Write(0u); \
                    LED_BLUE_Write (1u); \
                }while(0)

/* Set LED BLUE color */
#define RGB_LED_ON_BLUE \
                do{     \
                    LED_RED_Write  (1u); \
                    LED_GREEN_Write(1u); \
                    LED_BLUE_Write (0u); \
                }while(0)

/* Set LED TURN OFF */
#define RGB_LED_OFF \
                do{ \
                    LED_RED_Write  (1u); \
                    LED_GREEN_Write(1u); \
                    LED_BLUE_Write (1u); \
                }while(0)

#else
/* PSoC 3/PSoC 5LP: LED is active high on kit. */
    
/* Turn on LED */
#define TURN_ON_LED \
            do {     \
                LED4_Write(1u); \
            }while(0)

#define TURN_OFF_LED \
            do {     \
                LED4_Write(0u); \
            }while(0)

#endif /* (CY_PSOC4) */



#endif /* (CY_MAIN_H) */


/* [] END OF FILE */
