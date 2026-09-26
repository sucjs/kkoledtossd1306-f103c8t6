/**
 * @file kk_app.c
 * @brief KK_UI 应用入口：初始化顺序与主循环粘合。
 *
 * 顺序固定（见 kk-ui-port 的 porting-strategy）：
 *   OLED_Init() -> KK_UI_AppReset(now) -> KK_UI_Init() -> 循环内
 *   KK_UI_AppUpdate -> KK_UI_Update -> KK_UI_AppProcessEvents
 * 循环顺序不可颠倒；KK_UI_Update 每轮主循环调用一次，远快于 20 ms 绘制间隔。
 */
#include "kk_app.h"

#include "kk_app_port.h"
#include "kk_oled.h"
#include "kk_ui.h"
#include "kk_ui_app.h"

#include "main.h"

/** 显示故障后的最短重试间隔；避免总线持续异常时每轮循环都重画重发。 */
#define KK_APP_DISPLAY_RETRY_MS 1000U

static uint32_t s_display_retry_at;

void KK_App_Init(void)
{
    uint32_t now;

    /* 按键 GPIO 与 I2C2 的 DMA 发送通道必须在 OLED_Init 之前就绪。 */
    KK_App_InputInit();

    if (OLED_Init() != OLED_OK) {
        Error_Handler();
    }
    now = KK_App_Millis();
    KK_UI_AppReset(now);
    if (KK_UI_Init(KK_UI_AppGet()) != KK_UI_OK) {
        Error_Handler();
    }
    s_display_retry_at = now;
}

void KK_App_Loop(void)
{
    uint32_t now = KK_App_Millis();
    KK_UI_Input input;
    KK_UI_Status status;

    input.keys = KK_App_ReadKeys();
    input.encoder_delta = 0; /* 本工程使用三路 GPIO 按键，未接编码器。 */

    KK_UI_AppUpdate(now);
    status = KK_UI_Update(now, input);

    /*
     * 一次异步失败后屏幕内容不可信，需要允许重画并重发全屏。
     * KK_UI_RecoverDisplay 会立即把下一帧排到当前时刻，若总线仍异常就会每轮
     * 主循环都重试一次，因此这里用最小间隔限速；到点前的失败帧直接丢弃。
     */
    if (status == KK_UI_DISPLAY_ERROR || status == KK_UI_DISPLAY_TIMEOUT) {
        if ((int32_t)(now - s_display_retry_at) >= 0) {
            if (KK_UI_RecoverDisplay() == KK_UI_OK) {
                s_display_retry_at = now + KK_APP_DISPLAY_RETRY_MS;
            }
        }
    }

    KK_UI_AppProcessEvents();
}
