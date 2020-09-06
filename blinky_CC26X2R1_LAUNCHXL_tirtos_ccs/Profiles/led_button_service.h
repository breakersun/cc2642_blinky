/*
 * led_button_service.h
 *
 *  Created on: 2020Äê9ÔÂ4ÈÕ
 *      Author: ASUS
 */

#ifndef PROFILES_LED_BUTTON_SERVICE_H_
#define PROFILES_LED_BUTTON_SERVICE_H_

#include <bcomdef.h>

#define LBS_UUID_BASE(uuid)        0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                              0xDE, 0xEF, 0x12, 0x12, LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0x00
#define LBS_UUID_SERVICE     0x1523
#define LBS_UUID_BUTTON_CHAR 0x1524
#define LBS_UUID_LED_CHAR    0x1525

#define LBS_BUTTON_ID           0x01
#define LBS_BUTTON_LEN                1
#define LBS_BUTTON_LEN_MIN            1
#define LBS_LED_ID              0x02
#define LBS_LED_LEN                1
#define LBS_LED_LEN_MIN            1

// Callback when a characteristic value has changed
typedef void (*LedButtonServiceChange_t)(uint16_t connHandle, uint8_t paramID,
                                   uint16_t len, uint8_t *pValue);

typedef struct
{
    LedButtonServiceChange_t pfnChangeCb;          // Called when characteristic value changes
    LedButtonServiceChange_t pfnCfgChangeCb;       // Called when characteristic CCCD changes
} LedButtonServiceCBs_t;

extern bStatus_t LedButtonService_AddService(uint8_t rspTaskId);
extern bStatus_t LedButtonService_SetParameter(uint8_t param, uint16_t len, void *value);

#endif /* PROFILES_LED_BUTTON_SERVICE_H_ */
