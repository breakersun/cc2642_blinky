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

static uint8 BUTTONVal[LBS_BUTTON_LEN] = {0};

static uint16_t lbs_ButtonValLen = LBS_BUTTON_LEN_MIN;

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

static uint8 LEDVal[LBS_LED_LEN] = {0};

static uint16_t lbs_LEDValLen = LBS_LED_LEN_MIN;

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
        BUTTONVal
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
        LEDVal
    },
};

static LedButtonServiceCBs_t *pAppCBs = NULL;
static uint8_t lbs_icall_rsp_task_id = INVALID_TASK_ID;

static bStatus_t LB_Service_ReadAttrCB(uint16_t connHandle,
                                        gattAttribute_t *pAttr,
                                        uint8_t *pValue,
                                        uint16_t *pLen,
                                        uint16_t offset,
                                        uint16_t maxLen,
                                        uint8_t method);

bStatus_t LedButtonService_SetParameter(uint8_t param, uint16_t len, void *value)
{
    bStatus_t ret = SUCCESS;
    uint8_t  *pAttrVal;
    uint16_t *pValLen;
    uint16_t valMinLen;
    uint16_t valMaxLen;
    uint8_t sendNotiInd = FALSE;
    gattCharCfg_t *attrConfig;
    uint8_t needAuth;

    switch(param)
    {
    case LBS_LED_ID:
        pAttrVal = LEDVal;
        pValLen = &lbs_LEDValLen;
        valMinLen = LBS_LED_LEN_MIN;
        valMaxLen = LBS_LED_LEN;
        Log_info2("SetParameter : %s len: %d", (uintptr_t)"LED", len);
        break;

    case LBS_BUTTON_ID:
        pAttrVal = BUTTONVal;
        pValLen = &lbs_ButtonValLen;
        valMinLen = LBS_BUTTON_LEN_MIN;
        valMaxLen = LBS_BUTTON_LEN;
        sendNotiInd = TRUE;
        attrConfig = lbs_BUTTONConfig;
        needAuth = FALSE;  // Change if authenticated link is required for sending.
        Log_info2("SetParameter : %s len: %d", (uintptr_t)"BUTTON", len);
        break;

    default:
        Log_error1("SetParameter: Parameter #%d not valid.", param);
        return(INVALIDPARAMETER);
    }

    // Check bounds, update value and send notification or indication if possible.
    if(len <= valMaxLen && len >= valMinLen)
    {
        memcpy(pAttrVal, value, len);
        *pValLen = len; // Update length for read and get.

        if(sendNotiInd)
        {
            Log_info2("Trying to send noti/ind: connHandle %x, %s",
                      attrConfig[0].connHandle,
                      (uintptr_t)((attrConfig[0].value ==
                                   0) ? "\x1b[33mNoti/ind disabled\x1b[0m" :
                                  (attrConfig[0].value ==
                                   1) ? "Notification enabled" :
                                  "Indication enabled"));
            // Try to send notification.
            GATTServApp_ProcessCharCfg(attrConfig, pAttrVal, needAuth,
                                       LB_ServiceAttrTbl,
                                       GATT_NUM_ATTRS(LB_ServiceAttrTbl),
                                       lbs_icall_rsp_task_id,
                                       LB_Service_ReadAttrCB);
        }
    }
    else
    {
        Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", len,
                   valMinLen,
                   valMaxLen);
        ret = bleInvalidRange;
    }

    return(ret);
}

static uint8_t LedButton_Service_findCharParamId(gattAttribute_t *pAttr)
{
    // Is this a Client Characteristic Configuration Descriptor?
    if(ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID ==
       *(uint16_t *)pAttr->type.uuid)
    {
        return(LedButton_Service_findCharParamId(pAttr - 1)); // Assume the value attribute precedes CCCD and recurse
    }
    else if(ATT_UUID_SIZE == pAttr->type.len &&
            !memcmp(pAttr->type.uuid, buttonCharUUID, pAttr->type.len))
    {
        return(LBS_BUTTON_ID);
    }
    else if(ATT_UUID_SIZE == pAttr->type.len &&
            !memcmp(pAttr->type.uuid, ledCharUUID, pAttr->type.len))
    {
        return(LBS_LED_ID);
    }
    else
    {
        return(0xFF); // Not found. Return invalid.
    }
}


static bStatus_t LB_Service_ReadAttrCB(uint16_t connHandle,
                                        gattAttribute_t *pAttr,
                                        uint8_t *pValue,
                                        uint16_t *pLen,
                                        uint16_t offset,
                                        uint16_t maxLen,
                                        uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint16_t valueLen;
    uint8_t paramID = 0xFF;

    // Find settings for the characteristic to be read.
    paramID = LedButton_Service_findCharParamId(pAttr);
    switch(paramID)
    {
    case LBS_BUTTON_ID:
        valueLen = lbs_ButtonValLen;

        Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                  (uintptr_t)"BUTTON",
                  connHandle,
                  offset,
                  method);
        /* Other considerations for LED0 can be inserted here */
        break;

    case LBS_LED_ID:
        valueLen = lbs_LEDValLen;

        Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                  (uintptr_t)"LED",
                  connHandle,
                  offset,
                  method);
        /* Other considerations for LED1 can be inserted here */
        break;

    default:
        Log_error0("Attribute was not found.");
        return(ATT_ERR_ATTR_NOT_FOUND);
    }
    // Check bounds and return the value
    if(offset > valueLen)   // Prevent malicious ATT ReadBlob offsets.
    {
        Log_error0("An invalid offset was requested.");
        status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
        *pLen = MIN(maxLen, valueLen - offset); // Transmit as much as possible
        memcpy(pValue, pAttr->pValue + offset, *pLen);
    }

    return(status);
}

static bStatus_t LB_Service_WriteAttrCB(uint16_t connHandle,
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue,
                                         uint16_t len,
                                         uint16_t offset,
                                         uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint8_t paramID = 0xFF;
    uint8_t changeParamID = 0xFF;
    uint16_t writeLenMin;
    uint16_t writeLenMax;
    uint16_t *pValueLenVar;

    // See if request is regarding a Client Characterisic Configuration
    if(ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID ==
       *(uint16_t *)pAttr->type.uuid)
    {
        Log_info3("WriteAttrCB (CCCD): param: %d connHandle: %d %s",
                  LedButton_Service_findCharParamId(pAttr),
                  connHandle,
                  (uintptr_t)(method ==
                              GATT_LOCAL_WRITE ? "- restoring bonded state" :
                              "- OTA write"));

        // Allow notification and indication, but do not check if really allowed per CCCD.
        status = GATTServApp_ProcessCCCWriteReq(
            connHandle, pAttr, pValue, len,
            offset,
            GATT_CLIENT_CFG_NOTIFY |
            GATT_CLIENT_CFG_INDICATE);
        if(SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
        {
            pAppCBs->pfnCfgChangeCb(connHandle,
                                    LedButton_Service_findCharParamId(
                                        pAttr), len, pValue);
        }

        return(status);
    }

    // Find settings for the characteristic to be written.
    paramID = LedButton_Service_findCharParamId(pAttr);
    switch(paramID)
    {
    case LBS_LED_ID:
        writeLenMin = LBS_LED_LEN_MIN;
        writeLenMax = LBS_LED_LEN;
        pValueLenVar = &lbs_LEDValLen;

        Log_info5(
            "WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
            (uintptr_t)"LED",
            connHandle,
            len,
            offset,
            method);
        /* Other considerations for LED0 can be inserted here */
        break;

    default:
        Log_error0("Attribute was not found.");
        return(ATT_ERR_ATTR_NOT_FOUND);
    }
    // Check whether the length is within bounds.
    if(offset >= writeLenMax)
    {
        Log_error0("An invalid offset was requested.");
        status = ATT_ERR_INVALID_OFFSET;
    }
    else if(offset + len > writeLenMax)
    {
        Log_error0("Invalid value length was received.");
        status = ATT_ERR_INVALID_VALUE_SIZE;
    }
    else if(offset + len < writeLenMin &&
            (method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ))
    {
        // Refuse writes that are lower than minimum.
        // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
        //       only be refused if this attribute is the last in the queue (method is execute).
        //       Otherwise, reliable writes are accepted and parsed piecemeal.
        Log_error0("Invalid value length was received.");
        status = ATT_ERR_INVALID_VALUE_SIZE;
    }
    else
    {
        // Copy pValue into the variable we point to from the attribute table.
        memcpy(pAttr->pValue + offset, pValue, len);

        // Only notify application and update length if enough data is written.
        //
        // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
        //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
        // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
        //       because the write fragments are concatenated before being sent here.
        if(offset + len >= writeLenMin)
        {
            changeParamID = paramID;
            *pValueLenVar = offset + len; // Update data length.
        }
    }

    // Let the application know something changed (if it did) by using the
    // callback it registered earlier (if it did).
    if(changeParamID != 0xFF)
    {
        if(pAppCBs && pAppCBs->pfnChangeCb)
        {
            pAppCBs->pfnChangeCb(connHandle, paramID, len + offset, pValue); // Call app function from stack task context.
        }
    }
    return(status);
}

CONST gattServiceCBs_t LB_ServiceCBs =
{
 LB_Service_ReadAttrCB, // Read callback function pointer
 LB_Service_WriteAttrCB, // Write callback function pointer
 NULL                     // Authorization callback function pointer
};

extern bStatus_t LedButtonService_AddService(uint8_t rspTaskId)
{
    uint8_t status;

    // Allocate Client Characteristic Configuration table
    lbs_BUTTONConfig = (gattCharCfg_t *)ICall_malloc(
        sizeof(gattCharCfg_t) * linkDBNumConns);
    if(lbs_BUTTONConfig == NULL)
    {
        return(bleMemAllocError);
    }

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(LB_ServiceAttrTbl,
                                         GATT_NUM_ATTRS(LB_ServiceAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &LB_ServiceCBs);
    Log_info2("Registered service, %d attributes, status 0x%02x",
              GATT_NUM_ATTRS(LB_ServiceAttrTbl), status);
    return(status);
}




