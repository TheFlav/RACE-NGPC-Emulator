//---------------------------------------------------------------------------
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version. See also the license.txt file for
//  additional informations.
//---------------------------------------------------------------------------

// state.h: state saving
//
//  09/11/2008 Initial version (Akop Karapetyan)
//
//////////////////////////////////////////////////////////////////////

#ifndef _STATE_H
#define _STATE_H

#ifndef __GP32__
#include "stdafx.h"
#endif
//#include "cz80.h"
#include "DrZ80.h"
#include "neopopsound.h"
#include "DrZ80_support.h"

//-----------------------------------------------------------------------------
// State Definitions:
//-----------------------------------------------------------------------------

#define INT_QUEUE_MAX 4

typedef struct
{
  //Save state version
  _u8 state_version; // = 0x10

  //Rom signature
  _u8 rom_signature[0x40];

	//Memory
	_u8 ram[0xc000];
  _u8 cpuram[0x08a0];// 0xC000]; 0x38000

	//TLCS-900h Registers
	_u32 pc, sr;
	_u8 f_dash;
	_u32 gpr[23];

  //Z80 Registers
  Z80_Regs Z80;
  _u32 PC_offset;
  //there's no need? maybe...
  //_s32 Z80_ICount;

  //Sound Chips
  int sndCycles;
  SoundChip toneChip;
  SoundChip noiseChip;

	//Timers
  int timer0, timer1, timer2, timer3;

	//DMA
  _u8 ldcRegs[64];
}
RACE_STATE;

BOOL state_store(char* filename);
BOOL state_restore(char* filename);
BOOL state_store(FILE *stream);
BOOL state_restore(FILE *stream);

//=============================================================================
#endif // _STATE_H

