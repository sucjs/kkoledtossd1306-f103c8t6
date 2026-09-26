#ifndef KK_APP_PORT_H
#define KK_APP_PORT_H

#include <stdint.h>

/*
 * 平台接线：把目标工程的时基、GPIO 按键和 OLED DMA 回调归一化为 KK_UI 输入。
 * 只在本文件的实现里引用 HAL，业务与 UI 代码不直接依赖它。
 */

enum {
    KK_APP_KEY_UP = 1U << 0,
    KK_APP_KEY_DOWN = 1U << 1,
    KK_APP_KEY_OK = 1U << 2
};

/** 累计毫秒时基，由 SysTick 中断推进；返回 1 ms 计数的当前值。 */
uint32_t KK_App_Millis(void);

/** SysTick 中断入口调用，每次推进 1 ms。 */
void KK_App_TickInc(void);

/** 复位按键去抖状态；按键引脚在 gpio.c，I2C2 DMA 通道在 i2c.c/dma.c 配置。 */
void KK_App_InputInit(void);

/** 读取一次按键并做软件去抖，返回 KK_APP_KEY_* 的按位或。 */
uint8_t KK_App_ReadKeys(void);

/** 由 HAL_I2C_MemTxCpltCallback / HAL_I2C_ErrorCallback 转发。 */
void KK_App_OnI2cMemTxComplete(void);
void KK_App_OnI2cError(void);

#endif
