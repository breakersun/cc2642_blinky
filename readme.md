

## **LED Button Service Server**



### UUID

```c
#define LBS_UUID_BASE        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                              0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define LBS_UUID_SERVICE     0x1523
#define LBS_UUID_BUTTON_CHAR 0x1524
#define LBS_UUID_LED_CHAR    0x1525
                              
```



### Service Table

| Char                 | Properties  | Size    | Value                                         |
| -------------------- | ----------- | ------- | --------------------------------------------- |
| LBS_UUID_BUTTON_CHAR | read/notify | uint8_t | 0x00: Button Pressed<br>0x01: Button Released |
| LBS_UUID_LED_CHAR    | read/write  | uint8_t | 0x00: LED Disabled<br>0x01: LED Enabled       |



### Advertising Component



| LEN  | TYPE | VALUE         | PacketType   |
| ---- | ---- | ------------- | ------------ |
| 3    | 0x19 | 0x0000        | Source Adv   |
| 2    | 0x01 | 0x06          | Source Adv   |
| 14   | 0x09 | Nordic_Blinky | Source Adv   |
| 17   | 0x07 | LBS_UUID_BASE | Response Adv |

