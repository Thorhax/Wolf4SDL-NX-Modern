#ifndef _SWITCH_INPUT_H_
#define _SWITCH_INPUT_H_

#ifdef __SWITCH__
#include <switch.h>

extern PadState switch_pad;

void Switch_InitInput();
void Switch_UpdateEvents();
void Switch_PollGameControls();

#endif
#endif
