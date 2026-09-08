#ifdef __SWITCH__
#include <switch.h>
#include <SDL2/SDL.h>
#include "wl_def.h"
#include "switch_input.h"

PadState switch_pad;
static bool input_initialized = false;

void Switch_InitInput()
{
    if (input_initialized)
        return;
    input_initialized = true;

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&switch_pad);
}

void Switch_UpdateEvents()
{
    Switch_InitInput();

    padUpdate(&switch_pad);
    u64 kDown = padGetButtonsDown(&switch_pad);
    u64 kUp = padGetButtonsUp(&switch_pad);
    HidAnalogStickState l_stick = padGetStickPos(&switch_pad, 0);

    auto sendKeyDown = [](ScanCode sc, char ascii = 0) {
        LastScan = sc;
        if (ascii) LastASCII = ascii;
        Keyboard[sc] = 1;
    };
    auto sendKeyUp = [](ScanCode sc) {
        Keyboard[sc] = 0;
    };

    static uint32_t lastStickTic = 0;
    static uint32_t stickPressTic = 0;
    static ScanCode stickNavKey = sc_None;
    uint32_t curTic = SDL_GetTicks();

    // D-Pad and Stick menu navigation: ONLY send keyboard arrow events
    // when in a menu or pause screen. During gameplay, sticks and D-Pad are
    // handled natively in Switch_PollGameControls to prevent stuck keys and conflicts.
    if (!ingame || menuactive || Paused)
    {
        if (kDown & HidNpadButton_Up)
            sendKeyDown(sc_UpArrow);
        else if (kUp & HidNpadButton_Up)
            sendKeyUp(sc_UpArrow);

        if (kDown & HidNpadButton_Down)
            sendKeyDown(sc_DownArrow);
        else if (kUp & HidNpadButton_Down)
            sendKeyUp(sc_DownArrow);

        if (kDown & HidNpadButton_Left)
            sendKeyDown(sc_LeftArrow);
        else if (kUp & HidNpadButton_Left)
            sendKeyUp(sc_LeftArrow);

        if (kDown & HidNpadButton_Right)
            sendKeyDown(sc_RightArrow);
        else if (kUp & HidNpadButton_Right)
            sendKeyUp(sc_RightArrow);

        // Left analog stick navigation in menus (repeat rate ~220ms, pulse 60ms)
        const int MENU_STICK_DEAD_ZONE = 18000;
        ScanCode currentStickKey = sc_None;
        if (l_stick.y > MENU_STICK_DEAD_ZONE)
            currentStickKey = sc_UpArrow;
        else if (l_stick.y < -MENU_STICK_DEAD_ZONE)
            currentStickKey = sc_DownArrow;
        else if (l_stick.x < -MENU_STICK_DEAD_ZONE)
            currentStickKey = sc_LeftArrow;
        else if (l_stick.x > MENU_STICK_DEAD_ZONE)
            currentStickKey = sc_RightArrow;

        if (currentStickKey != sc_None)
        {
            if (currentStickKey != stickNavKey)
            {
                if (stickNavKey != sc_None)
                    sendKeyUp(stickNavKey);
                stickNavKey = currentStickKey;
                stickPressTic = curTic;
                lastStickTic = curTic;
                sendKeyDown(currentStickKey);
            }
            else
            {
                // Repeat logic
                if (curTic - lastStickTic > 220)
                {
                    lastStickTic = curTic;
                    stickPressTic = curTic;
                    sendKeyDown(currentStickKey);
                }
                else if (curTic - stickPressTic > 60)
                {
                    sendKeyUp(currentStickKey);
                }
            }
        }
        else
        {
            if (stickNavKey != sc_None)
            {
                sendKeyUp(stickNavKey);
                stickNavKey = sc_None;
            }
        }
        // A / ZR = Enter / Confirm
        if (kDown & (HidNpadButton_A | HidNpadButton_ZR))
            sendKeyDown(sc_Return, 13);
        else if (kUp & (HidNpadButton_A | HidNpadButton_ZR))
            sendKeyUp(sc_Return);

        // B / Minus / Plus = Escape / Back
        if (kDown & (HidNpadButton_B | HidNpadButton_Minus | HidNpadButton_Plus))
            sendKeyDown(sc_Escape, 27);
        else if (kUp & (HidNpadButton_B | HidNpadButton_Minus | HidNpadButton_Plus))
            sendKeyUp(sc_Escape);

        // Y = Yes, X = No (for prompts like "Are you sure? (Y/N)")
        if (kDown & HidNpadButton_Y)
            sendKeyDown(sc_Y, 'Y');
        else if (kUp & HidNpadButton_Y)
            sendKeyUp(sc_Y);

        if (kDown & HidNpadButton_X)
            sendKeyDown(sc_N, 'N');
        else if (kUp & HidNpadButton_X)
            sendKeyUp(sc_N);
    }
    else
    {
        // When in game: ensure stick menu key is cleared and arrow keys are never stuck
        if (stickNavKey != sc_None)
        {
            sendKeyUp(stickNavKey);
            stickNavKey = sc_None;
        }
        Keyboard[sc_UpArrow] = 0;
        Keyboard[sc_DownArrow] = 0;
        Keyboard[sc_LeftArrow] = 0;
        Keyboard[sc_RightArrow] = 0;
        Keyboard[sc_Return] = 0;
        Keyboard[sc_Escape] = 0;
    }
}

void Switch_PollGameControls()
{
    Switch_InitInput();

    padUpdate(&switch_pad);
    u64 kHeld = padGetButtons(&switch_pad);
    u64 kDown = padGetButtonsDown(&switch_pad);

    // Primary action buttons
    if ((kHeld & HidNpadButton_A) || (kHeld & HidNpadButton_ZR))
        buttonstate[bt_attack] = true;

    if (kHeld & HidNpadButton_B)
        buttonstate[bt_use] = true;

    if (kHeld & HidNpadButton_X)
        buttonstate[bt_strafe] = true;

    if ((kHeld & HidNpadButton_Y) || (kHeld & HidNpadButton_ZL))
        buttonstate[bt_run] = true;

    // Weapon switching
    if (kDown & HidNpadButton_R)
        buttonstate[bt_nextweapon] = true;

    if (kDown & HidNpadButton_L)
        buttonstate[bt_prevweapon] = true;

    if (kDown & HidNpadButton_StickR)
        buttonstate[bt_nextweapon] = true;

    // System keys
    if (kDown & HidNpadButton_Minus)
        buttonstate[bt_esc] = true;

    if (kDown & HidNpadButton_Plus)
        buttonstate[bt_pause] = true;

    // Left stick click: toggle always run
    if (kDown & HidNpadButton_StickL)
        always_run = !always_run;

    // D-Pad digital movement (Up/Down: move forward/backward, Left/Right: strafe left/right)
    int delta = (buttonstate[bt_run] ^ always_run) ? RUNMOVE * tics : BASEMOVE * tics;
    if (kHeld & HidNpadButton_Up)
        controly -= delta;
    if (kHeld & HidNpadButton_Down)
        controly += delta;
    if (kHeld & HidNpadButton_Left)
        controlstrafe -= delta;
    if (kHeld & HidNpadButton_Right)
        controlstrafe += delta;

    // Dual Analog Sticks
    HidAnalogStickState pos_left = padGetStickPos(&switch_pad, 0);
    HidAnalogStickState pos_right = padGetStickPos(&switch_pad, 1);

    const int DEAD_ZONE = 4000;
    const int MAX_ZONE = 28000;

    // Left Stick: Analog forward / backward (GZDoom style)
    if (pos_left.y > DEAD_ZONE)
    {
        float speed = (float)(pos_left.y - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        int move = (int)(speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controly -= move;
    }
    else if (pos_left.y < -DEAD_ZONE)
    {
        float speed = (float)(-pos_left.y - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        int move = (int)(speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controly += move;
    }

    // Left Stick: Analog strafe left / right (GZDoom style)
    if (pos_left.x > DEAD_ZONE)
    {
        float speed = (float)(pos_left.x - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        int strafe = (int)(speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controlstrafe += strafe;
    }
    else if (pos_left.x < -DEAD_ZONE)
    {
        float speed = (float)(-pos_left.x - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        int strafe = (int)(speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controlstrafe -= strafe;
    }

    // Right Stick X: Analog turning (2x speed, smooth quadratic curve)
    if (pos_right.x < -DEAD_ZONE)
    {
        float speed = (float)(-pos_right.x - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        speed = speed * speed; // quadratic ramp for aiming precision
        int turn = (int)(2.0f * speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controlx -= turn;
    }
    else if (pos_right.x > DEAD_ZONE)
    {
        float speed = (float)(pos_right.x - DEAD_ZONE) / (float)(MAX_ZONE - DEAD_ZONE);
        if (speed > 1.0f) speed = 1.0f;
        speed = speed * speed;
        int turn = (int)(2.0f * speed * (buttonstate[bt_run] ^ always_run ? RUNMOVE : BASEMOVE) * tics);
        controlx += turn;
    }
}
#endif
