
/***************************************************************************************
****************************************************************************************
* file    : onewire.c
* brief   : 
* emai    :			  
* Copyright (c) 2020-16-09 by TCL. All Rights Reserved.
* 
* history :
* version		Name       		Date			Description
   
****************************************************************************************
****************************************************************************************/


/* ------ Includes ------------------------------------------------------------------*/
#include "onewire.h"
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/* ------ External variables --------------------------------------------------------*/

/* ------ Private macros ------------------------------------------------------------*/
/* ------ Private typedef -----------------------------------------------------------*/

/* ------ Private variables ---------------------------------------------------------*/
static PIN_Handle oneWirePinHandle;
static PIN_Id oneWirePinId;
/* ------ Private functions ---------------------------------------------------------*/
static __inline void io_set(bool state)
{
    PIN_setOutputValue(oneWirePinHandle, oneWirePinId, state);
}

static __inline void delay_ms(uint32_t ms)
{
    Task_sleep(ms * (1000/Clock_tickPeriod));
}

static __inline void stop(void)
{
    io_set(0);
}

static __inline void start(void)
{
    io_set(1);
    delay_ms(20);
    io_set(0);
    delay_ms(2);
}




/************************************************************************************
* Function	: OneWire_Init
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value: 
************************************************************************************/
void OneWire_Init(PIN_Handle pin, PIN_Id pin_id)
{
    oneWirePinHandle    = pin;
    oneWirePinId        = pin_id;

    stop();
}



/************************************************************************************
* Function	: OneWire_SendByte
* Description	: 
* Input Para	: 
* Output Para	: 
* Return Value: 
************************************************************************************/
void OneWire_SendByte(uint8_t msg)
{
    start();
}


