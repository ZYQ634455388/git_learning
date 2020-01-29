#ifndef __Ledcon_H
#define __Ledcon_H
extern u8 led_str[5];
void Out595(u8 *str);
void Led_74ls595(u8 light, u8 channel);
void Led_Control_Config(void);
#endif

