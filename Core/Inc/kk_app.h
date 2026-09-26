#ifndef KK_APP_H
#define KK_APP_H

/** 初始化按键/DMA、OLED、KK_UI 应用与运行时；由 main() 在 MX_I2C2_Init 之后调用。 */
void KK_App_Init(void);

/** 主循环单次迭代：采样输入、更新应用与 UI、处理业务事件。 */
void KK_App_Loop(void);

#endif
