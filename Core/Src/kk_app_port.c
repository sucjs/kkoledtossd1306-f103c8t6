/**
 * @file kk_app_port.c
 * @brief KK_UI 平台接线：毫秒时基与 GPIO 按键去抖。
 *
 * 硬件事实：
 * - 按键：PA0=上、PA1=下、PA2=确定，内部上拉、低电平有效；引脚配置在 gpio.c。
 * - OLED：I2C2（PB10/PB11），发送 DMA 使用 DMA1 Channel4；配置在 i2c.c / dma.c。
 */
#include "kk_app_port.h"

#include "kk_oled_driver.h"

#include "main.h"

/** 按键引脚与去抖参数。 */
#define KK_APP_KEY_PORT         GPIOA
#define KK_APP_KEY_UP_PIN       GPIO_PIN_0
#define KK_APP_KEY_DOWN_PIN     GPIO_PIN_1
#define KK_APP_KEY_OK_PIN       GPIO_PIN_2
/** 同一电平连续保持该毫秒数后才确认为稳定状态。 */
#define KK_APP_KEY_DEBOUNCE_MS  5U

static volatile uint32_t s_millis;

/* 每路按键的去抖状态：稳定电平、最近原始电平、原始电平最近变化时刻。 */
static uint8_t s_key_stable[3];
static uint8_t s_key_raw[3];
static uint32_t s_key_changed_at[3];

uint32_t KK_App_Millis(void)
{
    return s_millis;
}

void KK_App_TickInc(void)
{
    ++s_millis;
}

static uint8_t kk_app_pin_pressed(uint16_t pin)
{
    /* 内部上拉、低电平有效：读到低电平表示按下。 */
    return (HAL_GPIO_ReadPin(KK_APP_KEY_PORT, pin) == GPIO_PIN_RESET) ? 1U : 0U;
}

void KK_App_InputInit(void)
{
    for (uint8_t i = 0U; i < 3U; ++i) {
        s_key_stable[i] = 0U;
        s_key_raw[i] = 0U;
        s_key_changed_at[i] = 0U;
    }
}

/**
 * 读取三路按键并做逐路软件去抖。
 * 原始电平每次变化都重置计时，只有同一电平连续保持达到去抖时长才更新稳定电平；
 * KK_UI 自己负责边沿识别、首次重复延迟和连发节奏。
 */
uint8_t KK_App_ReadKeys(void)
{
    static const uint16_t pins[3] = {
        KK_APP_KEY_UP_PIN, KK_APP_KEY_DOWN_PIN, KK_APP_KEY_OK_PIN
    };
    static const uint8_t masks[3] = {
        KK_APP_KEY_UP, KK_APP_KEY_DOWN, KK_APP_KEY_OK
    };
    uint32_t now = s_millis;
    uint8_t keys = 0U;

    for (uint8_t i = 0U; i < 3U; ++i) {
        uint8_t raw = kk_app_pin_pressed(pins[i]);

        if (raw != s_key_raw[i]) {
            s_key_raw[i] = raw;
            s_key_changed_at[i] = now;
        } else if ((now - s_key_changed_at[i]) >= KK_APP_KEY_DEBOUNCE_MS) {
            /* 同一电平已连续保持足够久，确认为稳定状态。 */
            s_key_stable[i] = raw;
        } else {
            /* 仍在去抖窗口内，保持上一个稳定值。 */
        }

        if (s_key_stable[i] != 0U) {
            keys |= masks[i];
        }
    }
    return keys;
}

void KK_App_OnI2cMemTxComplete(void)
{
    OLED_DriverHandleMemTxComplete();
}

void KK_App_OnI2cError(void)
{
    OLED_DriverHandleError();
}
