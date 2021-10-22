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

void printr(const char* tag, const char* txt) {
    char msg[80];
    snprintf(msg, 80, "[%-4s]: %-60s\r", tag, txt);
    print(msg);
}

void println(const char* tag, const char* txt) {
    char msg[80];
    snprintf(msg, 80, "[%-4s]: %-60s\r\n", tag, txt);
    print(msg);
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
    size_t   size;
    uint64_t data;
    uint32_t cntr;
    uint32_t addr;
    char     msg[100];

    /* Mount SD card */
    printr("SD", "Mounting");
    fr = f_mount(&USERFatFS, (TCHAR const*)USERPath, 1);
    if (fr != FR_OK) {
        /* f_mount failed */
        println("SD", "Cannot be mounted");
        sprintf(msg, "FatFs error code: %u", fr);
        println("SD", msg);
        return ERR_SD_MOUNT;
    }
    println("SD", "Mounted");

    /* Open file for programming */
    printr("FILE", "Loading");
    fr = f_open(&USERFile, CONF_FILENAME, FA_READ);
    if (fr != FR_OK) {
        uint8_t res;

        if (fr == FR_NO_FILE) {
            println("FILE", "Nothing to flash");
            res = ERR_OK;

        } else {
            /* f_open failed */
            println("FILE", "Cannot be opened");
            sprintf(msg, "FatFs error code: %u", fr);
            println("FILE", msg);
            res = ERR_SD_FILE;
        }

        SD_Eject();
        println("SD", "Ejected");
        return res;
    }
    println("FILE", "Found");

    /* Check size of application found on SD card */
    printr("SIZE", "Checking size");
    size = f_size(&USERFile);
    if (Bootloader_CheckSize(size) != BL_OK) {
        println("SIZE", "Error: too big");
        f_close(&USERFile);
        SD_Eject();
        println("SD", "Ejected");
        return ERR_APP_LARGE;
    }
    println("SIZE", "App size OK");

    /* Step 1: Init Bootloader and Flash */
    Bootloader_Init();

    /* Step 2: Erase Flash */
    printr("ERAZ", "Erasing flash...");
    LED_G2_ON();
    Bootloader_Erase();
    LED_G2_OFF();
    println("ERAZ", "Flash erased");

    /* Step 3: Programming */
    printr("PROG", "Starting");
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
                snprintf(msg, 50, "Error at: %lu byte", (cntr * 8));
                println("PROG", msg);

                f_close(&USERFile);
                SD_Eject();
                println("SD", "Ejected");

                LED_ALL_OFF();
                return ERR_FLASH;
            }
        }
        if (cntr % 256 == 0) {
            LED_G2_TG();
            snprintf(msg, 50, "%2lu%% [%6lu/%6u]", cntr * 8 * 100 / size, cntr * 8, size);
            printr("PROG", msg);
        }
    } while ((fr == FR_OK) && (num > 0));

    /* Step 4: Finalize Programming */
    Bootloader_FlashEnd();
    f_close(&USERFile);
    LED_ALL_OFF();
    snprintf(msg, 50, "Flashed %d bytes", size);
    println("PROG", msg);

    /* Open file for verification */
    printr("CHCK", "Checking data");
    fr = f_open(&USERFile, CONF_FILENAME, FA_READ);
    if (fr != FR_OK) {
        /* f_open failed */
        println("FILE", "Cannot be re-opened");
        sprintf(msg, "FatFs error code: %u", fr);
        println("FILE", msg);

        SD_Eject();
        println("SD", "Ejected");
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
                snprintf(msg, 50, "Error at: %lu byte", cntr * 4);
                println("CHCK", msg);

                f_close(&USERFile);
                SD_Eject();
                println("SD", "Ejected");

                LED_G1_OFF();
                return ERR_VERIFY;
            }
        }
        if (cntr % 256 == 0) {
            /* Toggle green LED during verification */
            LED_G1_TG();
            snprintf(msg, 50, "%2lu%% [%6lu/%6u]", cntr * 4 * 100 / size, cntr * 4, size);
            printr("CHCK", msg);
        }
    } while ((fr == FR_OK) && (num > 0));
    println("CHCK", "Passed");
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
    printr("FILE", "Erasing firmware file");
    fr = f_unlink(CONF_FILENAME);
    if (fr != FR_OK) {
        println("FILE", "Failed to erase file");
        sprintf(msg, "FatFs error code: %u\r\n", fr);
        println("FILE", msg);
        return ERR_FILE_DELETE;
    }
    printr("FILE", "File erased");

    /* Eject SD card */
    SD_Eject();
    println("SD", "Ejected");
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
