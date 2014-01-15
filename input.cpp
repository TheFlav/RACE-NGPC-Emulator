// input.cpp: implementation of the input class.
//
//////////////////////////////////////////////////////////////////////

//Flavor - Convert from DirectInput to SDL/GP2X

#ifndef __GP32__
#include "StdAfx.h"
#endif
#include "main.h"
#include "input.h"
#include "memory.h"

#ifdef TARGET_GP2X
#include "SDL_gp2x.h"
#include "state.h"
#endif

#ifdef GIZMONDO
#include "gizmondo.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// address where the state of the input device(s) is stored
//unsigned char	*InputByte = get_address(0x00006F82);
unsigned char	ngpInputState = 0;
unsigned char	*InputByte = &ngpInputState;

#ifdef TARGET_GP2X
int GP2X_fs=0;
int GP2X_fss_cd=0;
#endif

#ifdef __GP32__
#define DIK_UP BUTTON_UP
#define DIK_DOWN BUTTON_DOWN
#define DIK_LEFT BUTTON_LEFT
#define DIK_RIGHT BUTTON_RIGHT
#define DIK_SPACE BUTTON_A
#define DIK_N BUTTON_B
#define DIK_S BUTTON_SELECT
#define DIK_O BUTTON_START
#else
#define DIK_UP SDLK_UP
#define DIK_DOWN SDLK_DOWN
#define DIK_LEFT SDLK_LEFT
#define DIK_RIGHT SDLK_RIGHT
#define DIK_SPACE SDLK_a
#define DIK_N SDLK_b
#define DIK_S SDLK_ESCAPE
#define DIK_O SDLK_SPACE
#endif

#define DOWN(x)	keystates[x]
#ifdef __GP32__
u16 keystates = 0;
#else
int fs;
Uint8 *keystates = NULL;
#endif

#ifdef __GP32__
void InitInput()
#else
BOOL InitInput(HWND hwnd)
#endif
{
#if !defined (__GP32__) && !defined (GIZMONDO)
    keystates = GP2X_GetKeyStateArray(NULL);
#endif

#ifdef GIZMONDO
    keystates = SDL_GetKeyState(NULL);
#endif

	// setup standard values for input
	// NGP/NGPC:
	m_sysInfo[NGP].InputKeys[KEY_UP]			= DIK_UP;
	m_sysInfo[NGP].InputKeys[KEY_DOWN]		= DIK_DOWN;
	m_sysInfo[NGP].InputKeys[KEY_LEFT]		= DIK_LEFT;
	m_sysInfo[NGP].InputKeys[KEY_RIGHT]		= DIK_RIGHT;
	m_sysInfo[NGP].InputKeys[KEY_BUTTON_A]	= DIK_SPACE;
	m_sysInfo[NGP].InputKeys[KEY_BUTTON_B]	= DIK_N;
	m_sysInfo[NGP].InputKeys[KEY_SELECT]		= DIK_O;	// Option button
	m_sysInfo[NGPC].InputKeys[KEY_UP]			= DIK_UP;
	m_sysInfo[NGPC].InputKeys[KEY_DOWN]		= DIK_DOWN;
	m_sysInfo[NGPC].InputKeys[KEY_LEFT]		= DIK_LEFT;
	m_sysInfo[NGPC].InputKeys[KEY_RIGHT]		= DIK_RIGHT;
	m_sysInfo[NGPC].InputKeys[KEY_BUTTON_A]	= DIK_SPACE;
	m_sysInfo[NGPC].InputKeys[KEY_BUTTON_B]	= DIK_N;
	m_sysInfo[NGPC].InputKeys[KEY_SELECT]		= DIK_O;	// Option button

#ifndef __GP32__
    return TRUE;
#endif

}

#ifdef __GP32__
extern "C"
{
	int gp_getButton();
	void clearScreen();
}
int zoom=0,zoomy=16;
#endif

void UpdateInputState()
{
#ifdef __GP32__
	int key = gp_getButton();

	if ((key & BUTTON_R) && (key & BUTTON_L))
    {
        m_bIsActive = FALSE;//Flavor exit emulation
        return;
    }

	if (key & BUTTON_R) {
		zoom ^= 1;
		while ((key=gp_getButton())&BUTTON_R);
		if (!zoom)
			clearScreen();
	}

	if (key & BUTTON_L) {
		if (key & BUTTON_DOWN) {
			if (zoomy<32)
				zoomy+=1;
			return;
		}
		if (key & BUTTON_UP) {
			if (zoomy>0)
				zoomy-=1;
			return;
		}
	}
	if(key & BUTTON_DOWN)
	    *InputByte = 0x02;
	else if(key & BUTTON_UP)
	    *InputByte = 0x01;
	else
	    *InputByte = 0;

	if(key & BUTTON_RIGHT)
	    *InputByte |= 0x08;
	else if(key & BUTTON_LEFT)
	    *InputByte |= 0x04;

    if (key & BUTTON_A)
        *InputByte|= 0x10;
    if (key & BUTTON_B)
        *InputByte|= 0x20;
    if (key & BUTTON_SELECT)
        *InputByte|= 0x40;
#else
    SDL_Event event;

#ifdef TARGET_GP2X
    while(SDL_PollEvent(&event))
    {
    }

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_R) && SDL_JoystickGetButton(joystick, GP2X_BUTTON_L))
    {
        m_bIsActive = FALSE;//Flavor exit emulation
        return;
    }


#ifndef TARGET_WIZ
	if (GP2X_fss_cd>0){
		GP2X_fss_cd--;
	}else{
		//if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_R))
		if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_Y))
		{
			SDL_Rect *area;

			if (GP2X_fs==0){
				area->x=80;
				area->y=44;
				area->w=160;
				area->h=152;
				GP2X_fs=1;
			}else{
				area->x=0;
				area->y=0;
				area->w=320;
				area->h=240;
				GP2X_fs=0;
			}

			SDL_GP2X_Display(area);
			GP2X_fss_cd=10;
		}
	}
#endif


	if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWN))
	{
	    *InputByte = 0x02;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UP))
	{
	    *InputByte = 0x01;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_RIGHT))
	{
	    *InputByte = 0x08;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_LEFT))
	{
	    *InputByte = 0x04;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT))
	{
	    *InputByte = 0x09;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT))
	{
	    *InputByte = 0x05;
	}
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT))
	{
	    *InputByte = 0x0A;
    }
	else if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT))
	{
	    *InputByte = 0x06;
	}
	else
        *InputByte = 0;

    if (SDL_JoystickIndex(joystickusb) == 1)
    {
        int x_axis = SDL_JoystickGetAxis (joystickusb, 0);
        int y_axis = SDL_JoystickGetAxis (joystickusb, 1);


        *InputByte = 0;
        if(y_axis > 0)
        {
            *InputByte |= 0x02;
        }
        if(y_axis < 0)
        {
            *InputByte |= 0x01;
        }
        if(x_axis > 0)
        {
            *InputByte |= 0x08;
        }
        if(x_axis < 0)
        {
            *InputByte |= 0x04;
        }

        if (SDL_JoystickGetButton(joystickusb, 0))
            *InputByte |= 0x10;
        if (SDL_JoystickGetButton(joystickusb, 1))
            *InputByte |= 0x20;
        if (SDL_JoystickGetButton(joystickusb, 2))
            *InputByte |= 0x40;
    }

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_A) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_X))
        *InputByte|= 0x10;
    //if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_B) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_Y))
    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_B))
        *InputByte|= 0x20;
    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_SELECT))
        *InputByte|= 0x40;

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLUP))
        increaseVolume();
    else if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLDOWN))
        decreaseVolume();

    //Added to test savestates
    char *fn = "test.sav";

    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_L))
        state_store(fn);
    if (SDL_JoystickGetButton(joystick, GP2X_BUTTON_R))
        state_restore(fn);
#else
#ifdef GIZMONDO
	while(SDL_PollEvent(&event))
	{
	}

	if (DOWN(SDLK_F1))
        m_bIsActive = FALSE;//Flavor exit emulation

	int hat= (SDL_JoystickGetHat(joystick,0));
	if (hat & SDL_HAT_LEFT)
	{
		*InputByte = 0x04;
	}
	else if (hat & SDL_HAT_RIGHT)
	{
		*InputByte = 0x08;
	}
	else if (hat & SDL_HAT_UP)
	{
		*InputByte = 0x01;
	}
	else if (hat & SDL_HAT_DOWN)
	{
		*InputByte = 0x02;
	}
	else
	{
		 *InputByte = 0;
	}

    if (SDL_JoystickGetButton(joystick, GIZ_PLAY))
        *InputByte|= 0x10;
    if (SDL_JoystickGetButton(joystick, GIZ_FORWARDS))
        *InputByte|= 0x20;
    if (SDL_JoystickGetButton(joystick, GIZ_STOP))
        *InputByte|= 0x40;
#else
    SYSTEMINFO *si;
    GP2X_SetKeyStates(); //make sure they're updated

    while(SDL_PollEvent(&event))
    {
    }

    if (DOWN(SDLK_r) && DOWN(SDLK_l))
        m_bIsActive = FALSE;//Flavor exit emulation


    si = &m_sysInfo[NGP];
    *InputByte = 0;
    if (DOWN(si->InputKeys[KEY_BUTTON_A]))
        *InputByte|= 0x10;
    if (DOWN(si->InputKeys[KEY_BUTTON_B]))
        *InputByte|= 0x20;
    if (DOWN(si->InputKeys[KEY_SELECT]))
        *InputByte|= 0x40;
    if (DOWN(si->InputKeys[KEY_UP]))
        *InputByte|= 0x01;
    if (DOWN(si->InputKeys[KEY_DOWN]))
        *InputByte|= 0x02;
    if (DOWN(si->InputKeys[KEY_LEFT]))
        *InputByte|= 0x04;
    if (DOWN(si->InputKeys[KEY_RIGHT]))
        *InputByte|= 0x08;

    if (DOWN(SDLK_KP_PLUS))
        increaseVolume();
    else if (DOWN(SDLK_KP_MINUS))
        decreaseVolume();
#endif
#endif
#endif
}

void FreeInput()
{

}
