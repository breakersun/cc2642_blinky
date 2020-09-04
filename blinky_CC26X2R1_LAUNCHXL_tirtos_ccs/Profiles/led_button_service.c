/*
 * led_button_service.c
 *
 *  Created on: 2020Äê9ÔÂ4ÈÕ
 *      Author: ASUS
 */
#include <string.h>

//#include <xdc/runtime/Log.h> // Comment this in to use xdc.runtime.Log
#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "led_button_service.h"

// LED_BUTTON_Service Service UUID
CONST uint8_t LedButtonServiceUUID[ATT_UUID_SIZE] =
{
     LBS_UUID_BASE(LBS_UUID_SERVICE)
};
// Service declaration
static CONST gattAttrType_t LBServiceDecl = { ATT_UUID_SIZE, LedButtonServiceUUID };

static uint8_t lbs_ButtonProps = GATT_PROP_NOTIFY | GATT_PROP_READ;
// BUTTON UUID
CONST uint8_t buttonCharUUID[ATT_UUID_SIZE] =
{
 LBS_UUID_BASE(LBS_UUID_BUTTON_CHAR)
};

static uint8 BUTTONVal = 0;

// Characteristic "BUTTON" Client Characteristic Configuration Descriptor
static gattCharCfg_t *lbs_BUTTONConfig;

// Characteristic "LED" Properties (for declaration)
static uint8_t lbs_LEDProps = GATT_PROP_READ | GATT_PROP_WRITE |
                              GATT_PROP_WRITE_NO_RSP;

// LED UUID
CONST uint8_t ledCharUUID[ATT_UUID_SIZE] =
{
 LBS_UUID_BASE(LBS_UUID_LED_CHAR)
};

static uint8 LEDVal = 0;

static gattAttribute_t LB_ServiceAttrTbl[] =
{
    // LED_BUTTON_Service Service Declaration
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0,
        (uint8_t *)&LBServiceDecl
    },
    // Button Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &lbs_ButtonProps
    },
    // Button Characteristic Value
    {
        { ATT_UUID_SIZE, buttonCharUUID },
        GATT_PERMIT_READ,
        0,
        &BUTTONVal
    },
    // Button CCCD
    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&lbs_BUTTONConfig
    },

    // LED Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &lbs_LEDProps
    },
    // LED Characteristic Value
    {
        { ATT_UUID_SIZE, ledCharUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
        0,
        &LEDVal
    },
};
