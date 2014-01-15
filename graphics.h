//---------------------------------------------------------------------------
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version. See also the license.txt file for
//	additional informations.
//---------------------------------------------------------------------------

// graphics.h: interface for the graphics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICS_H__EE4B1FE1_8EB2_11D3_8644_00A0241D2A65__INCLUDED_)
#define AFX_GRAPHICS_H__EE4B1FE1_8EB2_11D3_8644_00A0241D2A65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//actual NGPC
#define NGPC_SIZEX 160
#define NGPC_SIZEY 152

//render screen
#define SIZEX	320
#define SIZEY	240

//extern unsigned char bwPaletteDirty, spritePaletteDirty, frontPaletteDirty, backPaletteDirty;
//extern unsigned char bgoowDirty;
//extern unsigned int spritesDirty;


// graphics buffer will hold the screen transformed to full RGB colors used by the emulated system
//extern int *graphics_buffer;
//extern unsigned short	*drawBuffer;
//extern unsigned short	drawBuffer[SIZEX*SIZEY];
//extern unsigned char *drawBuffer;
//extern BITMAP myBitmap;

#ifndef __GP32__
BOOL graphics_init(HWND phWnd);
#else
BOOL graphics_init();
#endif
void graphics_blit(void);
void graphics_paint();
void graphics_cleanup();
void write_screenshot(FILE *f);
// new renderer (NeoGeo Pocket (Color))
#if defined(DO_FPS_DISPLAY)
void incFrameCount();
#endif
void graphicsBlitInit();
void graphicsBlitLine(unsigned char render);
void myGraphicsBlitLine(unsigned char render);
void graphicsBlitEnd();
// renderer for Gameboy Color
void gbcGraphicsBlitInit();
void gbcGraphicsBlitLine();

void setColPaletteEntry(unsigned char addr, unsigned short data);
void setBWPaletteEntry(unsigned char addr, unsigned short data);


//
// adventure vision stuff
//
void advGraphicsEnd();

void graphics_debug(FILE *f);

extern unsigned short palettes[16*4+16*4+16*4]; // placeholder for the converted palette
extern int    totalpalette[32*32*32];
#define NGPC_TO_SDL16(col) totalpalette[col]

#define setColPaletteEntry(addr, data) palettes[(addr)] = NGPC_TO_SDL16(data)
#define setBWPaletteEntry(addr, data) palettes[(addr)] = NGPC_TO_SDL16(data)

extern unsigned char *scanlineY;


//#define DO_FPS_DISPLAY

#endif // !defined(AFX_GRAPHICS_H__EE4B1FE1_8EB2_11D3_8644_00A0241D2A65__INCLUDED_)
