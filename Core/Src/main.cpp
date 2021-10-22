/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include "app.h"
#include "bootloader.h"
#include <string.h>
#include <stdio.h>
//#include "shared/services/filesystem.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
constexpr char VBAR[]  = "========================================";
constexpr char STM32[] = "L452";
constexpr char COMM[]  = "SD";

/* Private function prototypes -----------------------------------------------*/
void print_info();
void SystemClock_Config(void);
void DeInit(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_FATFS_Init();
    MX_USART1_UART_Init();
    MX_SPI3_Init();

    /* Infinite loop */
    print_info();
    if (Enter_Bootloader()) {
        print("Failed to prepare bootloader\r\n");
        Error_Handler();
    }


    if (Bootloader_CheckForApplication() == BL_OK) {
        print("Jumping to application\r\n");
        print(
          "\r\n"
          "--------------"
          "\r\n\r\n");
        DeInit();
        Bootloader_JumpToApplication();
    }

    print("No application in flash.\r\n");
    LED_G2_TG();
    while (1) {
        LED_G1_TG();
        HAL_Delay(BLINK_FAST);
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 1;
    RCC_OscInitStruct.PLL.PLLN            = 10;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

void DeInit(void) {
    MX_SPI3_DeInit();
    MX_USART1_UART_DeInit();
    MX_FATFS_DeInit();
    MX_GPIO_DeInit();
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    HAL_Delay(5000);
    NVIC_SystemReset();
    while (1) {
    }
}

void print_info() {
    char       version[50];
    const char info[100] =
      "\033c"    // Clear Terminal Screen
      "CEP BOOTLOADER\r\n"
      "[STM32]: %s\r\n"
      "[COMM ]: %s\r\n"
      "[VER  ]: %s\r\n"
      "%s";
    char msg[100];
    Bootloader_GetVersion_Print(version);
    snprintf(msg, 100, info, STM32, COMM, version, VBAR);
    print(msg);
    print("\r\n");
}
