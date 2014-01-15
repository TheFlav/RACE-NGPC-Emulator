//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

//
// gp2x.cpp holds most of the GP2X specific support
//

#include "GP2X.h"
#include "StdAfx.h"
#include <math.h>
#include "main.h"

SDL_Joystick *joystick;
SDL_Joystick *joystickusb;

static short mouseOn = 1;

void GP2X_MouseOn()
{
	SDL_ShowCursor(SDL_ENABLE);
	mouseOn = 1;
}

void GP2X_MouseOff()
{
	SDL_ShowCursor(SDL_DISABLE);
	mouseOn = 0;
}

SDLKey GP2X_to_SDLK(int gp2xButton)
{
	switch(gp2xButton)
	{
		case GP2X_BUTTON_CLICK:
			return SDLK_RETURN;
		case GP2X_BUTTON_START:
			return SDLK_ESCAPE;
		case GP2X_BUTTON_SELECT:
			return SDLK_SPACE;
		case GP2X_BUTTON_A:
			return SDLK_a;
		case GP2X_BUTTON_X:
			return SDLK_x;
		case GP2X_BUTTON_Y:
			return SDLK_y;
		case GP2X_BUTTON_L:
			return SDLK_l;
		case GP2X_BUTTON_R:
			return SDLK_r;
		case GP2X_BUTTON_VOLUP:
			return SDLK_PLUS;
		case GP2X_BUTTON_VOLDOWN:
			return SDLK_MINUS;
		default:
			break;
	}

	if(!mouseOn)
	{
		switch(gp2xButton)
		{
			//These are mapped to mouse movements
			case GP2X_BUTTON_B:
				return SDLK_b;
			case GP2X_BUTTON_RIGHT:
			case GP2X_BUTTON_DOWNRIGHT:
			case GP2X_BUTTON_UPRIGHT:
				return SDLK_RIGHT;
			case GP2X_BUTTON_LEFT:
			case GP2X_BUTTON_UPLEFT:
			case GP2X_BUTTON_DOWNLEFT:
				return SDLK_LEFT;
			case GP2X_BUTTON_DOWN:
				return SDLK_DOWN;
			case GP2X_BUTTON_UP:
				return SDLK_UP;

			default:
				break;
		}
	}

	return SDLK_HELP;  //some random useless key
}

Uint8 keystate[SDLK_LAST] = {0};

Uint8 *GP2X_GetKeyStateArray(int *numkeys)
{
#ifdef TARGET_GP2X
	if(numkeys != NULL)
		*numkeys = SDLK_LAST;

	return keystate;
#else
    return SDL_GetKeyState(numkeys);
#endif
}

//this needs to be called often to set up the keystate array
void GP2X_SetKeyStates()
{
#ifdef TARGET_GP2X
	Uint8 r, l, u, d, ur, ul, dr, dl;

    SDL_Event event;

    //make sure that the events are run
    while(SDL_PollEvent(&event))
    {
    }

	r  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_RIGHT);
	l  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_LEFT);
	d  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWN);
	u  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UP);
	ur = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT);
	ul = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT);
	dr = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT);
	dl = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT);


	keystate[SDLK_RIGHT] = r || ur || dr;
	keystate[SDLK_LEFT]	 = l || ul || dl;
	keystate[SDLK_DOWN]	 = d || dl || dr;
	keystate[SDLK_UP]	 = u || ul || ur;

	keystate[SDLK_ESCAPE]= SDL_JoystickGetButton(joystick, GP2X_BUTTON_START);
	keystate[SDLK_RETURN]= SDL_JoystickGetButton(joystick, GP2X_BUTTON_CLICK);
	keystate[SDLK_SPACE]= SDL_JoystickGetButton(joystick, GP2X_BUTTON_SELECT);
	keystate[SDLK_b] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_B);
	keystate[SDLK_a] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_A);
	keystate[SDLK_x] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_X);
	keystate[SDLK_y] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_Y);
	keystate[SDLK_l] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_L);
	keystate[SDLK_r] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_R);

	keystate[SDLK_PLUS]  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLUP);
	keystate[SDLK_MINUS] = SDL_JoystickGetButton(joystick, GP2X_BUTTON_VOLDOWN);
#endif
}

int GP2X_anybuttondown()
{
    int i;
    Uint8 *states;

    states = GP2X_GetKeyStateArray(NULL);

    for(i=0;i<SDLK_LAST;i++)
    {
        if(i!=SDLK_CAPSLOCK && i!=SDLK_NUMLOCK && i!=SDLK_SCROLLOCK && states[i])
            return 1;
    }
    return 0;
}

#define JUMPTEST 10
#define JUMPGAP  7

unsigned char GP2X_GetMouseState(int *mx, int *my)
{
	static int mouseX=0, mouseY=0;
	static int jumpX=0, jumpY=0;
	static int lastMouseX=-1, lastMouseY=-1;
	static Uint8 lastBDown = 0;
	Uint8 bDown = 0;
	Uint8 r, l, u, d, ur, ul, dr, dl;

	int retVal = 0;

	//if we're not pretending that the joystick is a mouse
	if(!mouseOn)
		return 0;

	r  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_RIGHT);
	l  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_LEFT);
	d  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWN);
	u  = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UP);
	ur = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT);
	ul = SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT);
	dr = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT);
	dl = SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT);

	if(r || ur || dr)
	{
		if(jumpX > 0)
			jumpX++;
		else
			jumpX = 1;

		if(jumpX > JUMPTEST)
			mouseX += JUMPGAP;
		else
			mouseX++;
	}
	else if(l || ul || dl)
	{
		if(jumpX < 0)
			jumpX--;
		else
			jumpX = -1;

		if(jumpX < -JUMPTEST)
			mouseX -= JUMPGAP;
		else
			mouseX--;
	}
	else
		jumpX = 0;


	if(d || dr || dl)
	{
		if(jumpY > 0)
			jumpY++;
		else
			jumpY = 1;

		if(jumpY > JUMPTEST)
			mouseY += JUMPGAP;
		else
			mouseY++;
	}
	else if(u || ur || ul)
	{
		if(jumpY < 0)
			jumpY--;
		else
			jumpY = -1;

		if(jumpY < -JUMPTEST)
			mouseY -= JUMPGAP;
		else
			mouseY--;
	}
	else
		jumpY = 0;

	bDown = SDL_JoystickGetButton(joystick, GP2X_BUTTON_X) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_CLICK);
	if(/*lastBDown && !*/bDown)
	{
		retVal = SDL_BUTTON(1);
	}
	lastBDown = bDown;

	if(mouseX < 0)
		mouseX = 0;
	if(mouseX > 319)
		mouseX = 319;
	if(mouseY < 0)
		mouseY = 0;
	if(mouseY > 239)
		mouseY = 239;


	if(mouseX != lastMouseX || mouseY != lastMouseY)
		SDL_WarpMouse(mouseX, mouseY);

	lastMouseX = mouseX;
	lastMouseY = mouseY;

	if(mx)
		*mx = mouseX;

	if(my)
		*my = mouseY;


	return retVal;
}

void GP2X_JoystickOpen()
{
    int numJoys = SDL_NumJoysticks();
    printf("GP2X_JoystickOpen:%d", numJoys);
    if (numJoys > 1)
    {
        joystick = SDL_JoystickOpen(0);
        joystickusb = SDL_JoystickOpen(1);
    } else {
        joystick = SDL_JoystickOpen(0);
    }
}

SDL_Surface *numbersGraphic = NULL;
unsigned char numbersWidth = 0, numbersHeight = 0;

void drawDigit(unsigned char digit, int x, int y)
{
    static bool firstRun = true;

    if(digit > 9)
        return;

    if(firstRun)
    {
        SDL_Surface *tempSurface;

        tempSurface = IMG_Load("Numbers.bmp");

        if(tempSurface)
        {
            numbersGraphic = SDL_DisplayFormat(tempSurface);
            SDL_FreeSurface(tempSurface);
        }

        firstRun = false;
    }

    if(numbersGraphic)
    {
        SDL_Rect src, dst;
        src.x = digit * (numbersGraphic->w/10);
        src.y = 0;
        dst.h = src.h = numbersGraphic->h;
        dst.w = src.w = numbersGraphic->w/10;

        dst.x = x;
        dst.y = y;


        if(SDL_BlitSurface(numbersGraphic, &src, screen, &dst) != 0)
            dbg_print("drawDigit:  SDL_BlitSurface failed\n");
    }
}

#define MAX_DIGITS 5
SDL_Rect drawNumber(unsigned int number, unsigned int x, unsigned int y)
{
    unsigned char digits[MAX_DIGITS];  //only handle 5 digits
    unsigned char currDigit = 0;
    SDL_Rect numRect;

    numRect.x = x;
    numRect.y = y;
    numRect.w = 0;
    numRect.h = 0;

    if(number == 0)
    {
        drawDigit(0, x, y);

        if(numbersGraphic)
        {
            numRect.h = numbersGraphic->h;
            numRect.w = numbersGraphic->w/10;
        }
        return numRect;
    }

    if(number > (pow(10, 5)-1))
        return numRect;

    for(currDigit = 0; currDigit < MAX_DIGITS && number > 0; currDigit++)
    {
        digits[currDigit] = number % 10;
        number /= 10;
    }

    if(numbersGraphic)
    {
        numRect.h = numbersGraphic->h;
        numRect.w = currDigit * (numbersGraphic->w/10);
    }

    while(currDigit)
    {
        currDigit--;
        drawDigit(digits[currDigit], x, y);
        x += numbersGraphic->w/10;
    }

    return numRect;
}
