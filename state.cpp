//---------------------------------------------------------------------------
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version. See also the license.txt file for
//  additional informations.
//---------------------------------------------------------------------------

// state.cpp: state saving
//
//  09/11/2008 Initial version (Akop Karapetyan)
//
//////////////////////////////////////////////////////////////////////

#include "state.h"
#include "tlcs900h.h"
#include "memory.h"
#include <string.h>

#ifdef PC
#undef PC
#endif

BOOL state_restore(FILE *stream)
{
  RACE_STATE rs;
  if (fread(&rs, sizeof(rs), 1, stream) < 1)
    return 0;

  // Verify ROM version
  if (rs.state_version != 0x10)
    return 0;

  // Verify ROM signature
  if (memcmp(mainrom, rs.rom_signature, sizeof(rs.rom_signature)) != 0)
    return 0;

  //TLCS-900h Registers
  gen_regsPC = rs.pc;
  gen_regsSR = rs.sr;
  F2 = rs.f_dash;

  int i = 0;
  gen_regsXWA0 = rs.gpr[i++];
  gen_regsXBC0 = rs.gpr[i++];
  gen_regsXDE0 = rs.gpr[i++];
  gen_regsXHL0 = rs.gpr[i++];

  gen_regsXWA1 = rs.gpr[i++];
  gen_regsXBC1 = rs.gpr[i++];
  gen_regsXDE1 = rs.gpr[i++];
  gen_regsXHL1 = rs.gpr[i++];

  gen_regsXWA2 = rs.gpr[i++];
  gen_regsXBC2 = rs.gpr[i++];
  gen_regsXDE2 = rs.gpr[i++];
  gen_regsXHL2 = rs.gpr[i++];

  gen_regsXWA3 = rs.gpr[i++];
  gen_regsXBC3 = rs.gpr[i++];
  gen_regsXDE3 = rs.gpr[i++];
  gen_regsXHL3 = rs.gpr[i++];

  gen_regsXIX = rs.gpr[i++];
  gen_regsXIY = rs.gpr[i++];
  gen_regsXIZ = rs.gpr[i++];
  gen_regsXSP = rs.gpr[i++];

  gen_regsSP = rs.gpr[i++];
  gen_regsXSSP = rs.gpr[i++];
  gen_regsXNSP = rs.gpr[i++];

  //Z80 Registers
  extern Z80_Regs *Z80p;
  extern Z80_Regs Z80;
  unsigned int size_of_z80 = sizeof(Z80);

  memcpy(Z80p, &rs.Z80, size_of_z80);
	//Need to make sure  don't have to set a different one...  
	//Z80_ICount = rs.Z80_ICount;
  Z80.regs.Z80PC = Z80.regs.z80_rebasePC(Z80.regs.Z80PC-Z80.regs.Z80PC_BASE);
  Z80.regs.Z80SP = Z80.regs.z80_rebaseSP(Z80.regs.Z80SP-Z80.regs.Z80SP_BASE);

/* Notaz' save state suggestions


    memcpy(&drZ80, data, 0x54);
    drZ80.Z80PC = drZ80.z80_rebasePC(Z80.regs.Z80PC-Z80.regs.Z80PC_BASE);
    drZ80.Z80SP = drZ80.z80_rebaseSP(Z80.regs.Z80SP-Z80.regs.Z80SP_BASE);

*/

  //Sound Chips
  extern int sndCycles;
  sndCycles = rs.sndCycles;
  memcpy(&toneChip, &rs.toneChip, sizeof(SoundChip));
  memcpy(&noiseChip, &rs.noiseChip, sizeof(SoundChip));

  //Timers
  timer0 = rs.timer0;
  timer1 = rs.timer1;
  timer2 = rs.timer2;
  timer3 = rs.timer3;

  //DMA
  memcpy(&ldcRegs, &rs.ldcRegs, sizeof(ldcRegs));

  //Memory
  memcpy(mainram, rs.ram, sizeof(rs.ram));
  memcpy(&mainram[0x20000], rs.cpuram, sizeof(rs.cpuram));

  tlcs_reinit();

  return 1;
}

//-----------------------------------------------------------------------------
// state_restore()
//-----------------------------------------------------------------------------
BOOL state_restore(char* filename)
{
  // Load
  FILE *stream;
  if (!(stream = fopen(filename, "r")))
    return 0;

  int status = state_restore(stream);
  fclose(stream);

  return status;
}

BOOL state_store(FILE *stream)
{
  RACE_STATE rs;

  //Build a state description
  rs.state_version = 0x10;

  //TLCS-900h Registers
  rs.pc = gen_regsPC;
  rs.sr = gen_regsSR;
  rs.f_dash = F2;

  int i = 0;
  rs.gpr[i++] = gen_regsXWA0;
  rs.gpr[i++] = gen_regsXBC0;
  rs.gpr[i++] = gen_regsXDE0;
  rs.gpr[i++] = gen_regsXHL0;

  rs.gpr[i++] = gen_regsXWA1;
  rs.gpr[i++] = gen_regsXBC1;
  rs.gpr[i++] = gen_regsXDE1;
  rs.gpr[i++] = gen_regsXHL1;

  rs.gpr[i++] = gen_regsXWA2;
  rs.gpr[i++] = gen_regsXBC2;
  rs.gpr[i++] = gen_regsXDE2;
  rs.gpr[i++] = gen_regsXHL2;

  rs.gpr[i++] = gen_regsXWA3;
  rs.gpr[i++] = gen_regsXBC3;
  rs.gpr[i++] = gen_regsXDE3;
  rs.gpr[i++] = gen_regsXHL3;

  rs.gpr[i++] = gen_regsXIX;
  rs.gpr[i++] = gen_regsXIY;
  rs.gpr[i++] = gen_regsXIZ;
  rs.gpr[i++] = gen_regsXSP;

  rs.gpr[i++] = gen_regsSP;
  rs.gpr[i++] = gen_regsXSSP;
  rs.gpr[i++] = gen_regsXNSP;

  //Z80 Registers
  extern Z80_Regs Z80;
  extern Z80_Regs *Z80p;
  int size_of_z80 = sizeof(Z80);

  memcpy(&rs.Z80, Z80p, size_of_z80);
	//Don't think I have this one...
	//rs.Z80_ICount = Z80_ICount;
  //rs.PC_offset = Z80_GetPC();

  //Sound Chips
  extern int sndCycles;
  rs.sndCycles = sndCycles;
  memcpy(&rs.toneChip, &toneChip, sizeof(SoundChip));
  memcpy(&rs.noiseChip, &noiseChip, sizeof(SoundChip));

  //Timers
  rs.timer0 = timer0;
  rs.timer1 = timer1;
  rs.timer2 = timer2;
  rs.timer3 = timer3;

  //DMA
  memcpy(&rs.ldcRegs, &ldcRegs, sizeof(ldcRegs));

  //Memory
  memcpy(rs.ram, mainram, sizeof(rs.ram));
  memcpy(rs.cpuram, &mainram[0x20000], sizeof(rs.cpuram));

  //ROM signature
  memcpy(rs.rom_signature, mainrom, sizeof(rs.rom_signature));

  // Write to file
  if (fwrite(&rs, sizeof(rs), 1, stream) < 1)
    return 0;

  return 1;
}

BOOL state_store(char* filename)
{
  // Save
  FILE *stream;
  if (!(stream = fopen(filename, "w")))
    return 0;

  int status = state_store(stream);
  fclose(stream);

  return status;
}
