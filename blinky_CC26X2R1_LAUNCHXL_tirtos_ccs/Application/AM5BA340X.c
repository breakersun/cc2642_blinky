
/***************************************************************************************
****************************************************************************************
* file    : AM5BA340X.c
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
/* ------ External variables --------------------------------------------------------*/

/* ------ Private macros ------------------------------------------------------------*/

/* ------ Private typedef -----------------------------------------------------------*/
enum
{
    PLAY = 0x01,
    LOOP = 0x02,
    STOP = 0x03,
    VOL  = 0x04
};
/* ------ Private variables ---------------------------------------------------------*/

/* ------ Private functions ---------------------------------------------------------*/

extern void AM5_Play(uint8_t musicId)
{
    OneWire_SendByte((PLAY << 4) | musicId);
}

extern void AM5_Loop(uint8_t musicId)
{
    OneWire_SendByte((LOOP << 4) | musicId);
}

extern void AM5_Stop(void)
{
    OneWire_SendByte((STOP << 4) | 0x00);
}

extern void AM5_SetVolume(uint8_t volume)
{
    OneWire_SendByte((VOL << 4) | volume);
}





