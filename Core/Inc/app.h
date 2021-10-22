#pragma once
#include <stdint.h>
#include "main.h"
#include "bootloader.h"

#define CONF_FILENAME "Scale.bin"

#define BLINK_FAST   100
#define BLINK_SLOW   500
#define LED_G1_ON()  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET)
#define LED_G1_OFF() HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET)
#define LED_G1_TG()  HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin)
#define LED_G2_ON()  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET)
#define LED_G2_OFF() HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET)
#define LED_G2_TG()  HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin)

#define LED_ALL_ON()                                                                                                   \
    do {                                                                                                               \
        LED_G1_ON();                                                                                                   \
        LED_G2_ON();                                                                                                   \
    } while (0)
#define LED_ALL_OFF()                                                                                                  \
    do {                                                                                                               \
        LED_G1_OFF();                                                                                                  \
        LED_G2_OFF();                                                                                                  \
    } while (0)
#define LED_ALL_TG()                                                                                                   \
    do {                                                                                                               \
        LED_G1_TG();                                                                                                   \
        LED_G2_TG();                                                                                                   \
    } while (0)


enum eApplicationErrorCodes
{
    ERR_OK = 0,
    ERR_WRP_ACTIVE,
    ERR_SD_INIT,
    ERR_SD_MOUNT,
    ERR_SD_FILE,
    ERR_APP_LARGE,
    ERR_FLASH,
    ERR_VERIFY,
    ERR_FILE_CLOSE,
    ERR_FILE_DELETE,
    ERR_OBP,
};


void    print(const char* str);
void    printr(const char* tag, const char* txt);
void    println(const char* tag, const char* txt);
bool    mount(void);
bool    unmount(void);
uint8_t Enter_Bootloader(void);
void    SD_Eject(void);
