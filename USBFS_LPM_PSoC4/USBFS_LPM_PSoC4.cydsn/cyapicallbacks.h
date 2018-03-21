/*******************************************************************************
* File Name: cyapicallbacks.h
*
* Version: 1.0
*
* Description:
*  This file provides function prototypes for the callbacks functions of
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

#ifndef CYAPICALLBACKS_H
#define CYAPICALLBACKS_H
    
#define USBFS_LPM_ISR_ENTRY_CALLBACK
void USBFS_LPM_ISR_EntryCallback(void);

#define USBFS_BUS_RESET_ISR_EXIT_CALLBACK
void  USBFS_BUS_RESET_ISR_ExitCallback(void);
    
#endif /* CYAPICALLBACKS_H */   
/* [] END OF FILE */
