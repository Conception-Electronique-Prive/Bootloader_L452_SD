# Bootloader for STM32L452
Original project: https://github.com/akospasztor/stm32-bootloader
 
Modified by CEP to support STM32L452


## Behavior
1. Initialize peripherals (HAL, Clock, GPIO, SPI, UART, FATFS)
2. Print Bootloader Information
3. Mount SD Card
4. On presence of firmware file on the SD card
   1. Erase Application space on Flash memory
   2. Write firmware file content on Flash memory
   3. Verify rightness of the written content
   4. Erase firmware file from SD card
   5. Any error in previous steps cancel the flashing procedure
5. Unmount SD Card
6. De-Initialize peripherals (HAL, Clock, GPIO, SPI, UART, FATFS)
7. Jump to application

## Requirements
The firmware file must be called `firmware.bin` and must be located at the root of the SD Card

On the application code (not bootloader)

In `system_stm32l4xx.c`, you must update `VECT_TAB_OFFSET` to `0x8000`

In `STM32L452RETX_FLASH.ld`, you must update the memory definition to 
```
/* Memories definition */
MEMORY
{
  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 160K
  FLASH    (rx)    : ORIGIN = 0x8008000,   LENGTH = 480K
}
``` 



