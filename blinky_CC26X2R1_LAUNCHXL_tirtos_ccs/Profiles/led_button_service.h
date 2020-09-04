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

extern bStatus_t LedButtonService_AddService(uint8_t rspTaskId);

#endif /* PROFILES_LED_BUTTON_SERVICE_H_ */
