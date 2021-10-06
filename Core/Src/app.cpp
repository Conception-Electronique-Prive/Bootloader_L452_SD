#include "app.h"
#include "usart.h"
#include "fatfs.h"
#include "ff.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief  Debug over UART2 -> ST-LINK -> USB Virtual Com Port
 * @param  str: string to be written to UART2
 * @retval None
 */
void print(const char* str) {
    HAL_UART_Transmit(&huart1, (uint8_t*)str, (uint16_t)strlen(str), 100);
}


/**
 * @brief  This function executes the bootloader sequence.
 * @param  None
 * @retval Application error code ::eApplicationErrorCodes
 *
 */
uint8_t Enter_Bootloader(void) {
    FRESULT  fr;
    UINT     num;
    uint8_t  status;
    uint64_t data;
    uint32_t cntr;
    uint32_t addr;
    char     msg[40] = {0x00};

    /* Mount SD card */
    fr = f_mount(&USERFatFS, (TCHAR const*)USERPath, 1);
    if (fr != FR_OK) {
        /* f_mount failed */
        print("SD card cannot be mounted.\r\n");
        sprintf(msg, "FatFs error code: %u\r\n", fr);
        print(msg);
        return ERR_SD_MOUNT;
    }
    print("SD mounted.\r\n");

    /* Open file for programming */
    fr = f_open(&USERFile, CONF_FILENAME, FA_READ);
    if (fr != FR_OK) {
        uint8_t res;

        if (fr == FR_NO_FILE) {
            print("No file to flash.\r\n");
            res = ERR_OK;

        } else {
            /* f_open failed */
            print("File cannot be opened.\r\n");
            sprintf(msg, "FatFs error code: %u\r\n", fr);
            print(msg);
            res = ERR_SD_FILE;
        }

        SD_Eject();
        print("SD ejected.\r\n");
        return res;
    }
    print("Software found on SD.\r\n");

    /* Check size of application found on SD card */
    if (Bootloader_CheckSize(f_size(&USERFile)) != BL_OK) {
        print("Error: app on SD card is too large.\r\n");

        f_close(&USERFile);
        SD_Eject();
        print("SD ejected.\r\n");
        return ERR_APP_LARGE;
    }
    print("App size OK.\r\n");

    /* Step 1: Init Bootloader and Flash */
    Bootloader_Init();

    /* Step 2: Erase Flash */
    print("Erasing flash...\r\n");
    LED_G2_ON();
    Bootloader_Erase();
    LED_G2_OFF();
    print("Flash erase finished.\r\n");

    /* Step 3: Programming */
    print("Starting programming...\r\n");
    LED_G1_ON();
    cntr = 0;
    Bootloader_FlashBegin();
    do {
        data = 0xFFFFFFFFFFFFFFFF;
        fr   = f_read(&USERFile, &data, 8, &num);
        if (num) {
            status = Bootloader_FlashNext(data);
            if (status == BL_OK) {
                cntr++;
            } else {
                sprintf(msg, "Programming error at: %lu byte\r\n", (cntr * 8));
                print(msg);

                f_close(&USERFile);
                SD_Eject();
                print("SD ejected.\r\n");

                LED_ALL_OFF();
                return ERR_FLASH;
            }
        }
        if (cntr % 256 == 0) {
            /* Toggle green LED during programming */
            LED_G2_TG();
        }
    } while ((fr == FR_OK) && (num > 0));

    /* Step 4: Finalize Programming */
    Bootloader_FlashEnd();
    f_close(&USERFile);
    LED_ALL_OFF();
    print("Programming finished.\r\n");
    sprintf(msg, "Flashed: %lu bytes.\r\n", (cntr * 8));
    print(msg);

    /* Open file for verification */
    fr = f_open(&USERFile, CONF_FILENAME, FA_READ);
    if (fr != FR_OK) {
        /* f_open failed */
        print("File cannot be opened.\r\n");
        sprintf(msg, "FatFs error code: %u\r\n", fr);
        print(msg);

        SD_Eject();
        print("SD ejected.");
        return ERR_SD_FILE;
    }

    /* Step 5: Verify Flash Content */
    addr = APP_ADDRESS;
    cntr = 0;
    do {
        data = 0xFFFFFFFFFFFFFFFF;
        fr   = f_read(&USERFile, &data, 4, &num);
        if (num) {
            if (*(uint32_t*)addr == (uint32_t)data) {
                addr += 4;
                cntr++;
            } else {
                sprintf(msg, "Verification error at: %lu byte.\r\n", (cntr * 4));
                print(msg);

                f_close(&USERFile);
                SD_Eject();
                print("SD ejected.\r\n");

                LED_G1_OFF();
                return ERR_VERIFY;
            }
        }
        if (cntr % 256 == 0) {
            /* Toggle green LED during verification */
            LED_G1_TG();
        }
    } while ((fr == FR_OK) && (num > 0));
    print("Verification passed.\r\n");
    LED_G1_OFF();

    /* Closing file */
    fr = f_close(&USERFile);
    if (fr != FR_OK) {
        print("Failed to close file.\r\n");
        sprintf(msg, "FatFs error code: %u\r\n", fr);
        print(msg);
        return ERR_FILE_CLOSE;
    }

    /* Erasing firmware */
    print("Erasing firmware file.\r\n");
    fr = f_unlink(CONF_FILENAME);
    if (fr != FR_OK) {
        print("Failed to erase file.\r\n");
        sprintf(msg, "FatFs error code: %u\r\n", fr);
        print(msg);
        return ERR_FILE_DELETE;
    }

    /* Eject SD card */
    SD_Eject();
    print("SD ejected.\r\n");
    return ERR_OK;
}

/**
 * @brief  This function ejects the SD card.
 * @param  None
 * @retval None
 */
void SD_Eject(void) {
    f_mount(NULL, (TCHAR const*)USERPath, 0);
}
