#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include <ti/drivers/PIN.h>

extern void OneWire_SendByte(uint8_t msg);
extern void OneWire_Init(PIN_Handle pin, PIN_Id pin_id);



#endif /*_ONEWIRE_H_*/

