#include "gfx.h"
#include <stdlib.h>
#include <string.h>

u32 framebuffers[3];
u32 nflip = 0;

#define SIZE (320*240*2)

void initGfx()
{
	int i;
	for (i=0;i<3;i++)
	{
		framebuffers[i] = (u32)malloc(SIZE+4096);
		framebuffers[i] = (framebuffers[i]+4095)&0xfffff000;
		gp_setMMU(framebuffers[i], ((unsigned char*)framebuffers[i])+SIZE-1,0xFFA);
	}
	gp_initFramebuffer((void*)framebuffers[0],16,85);
	clearScreen();
	nflip = 1;
}

int console_write_y = 2;

void clearScreen()
{
	memset((void*)framebuffers[0],0,SIZE);
	memset((void*)framebuffers[1],0,SIZE);
	memset((void*)framebuffers[2],0,SIZE);
	console_write_y = 2;
}

void flip(u32 sync)
{
	gp_setFramebuffer((void*)framebuffers[nflip],sync);
	if (++nflip==3)
		nflip=0;
}

void error(char *s)
{
	memset((void*)framebuffers[nflip],0xff,SIZE);
	gp_drawString(20,20,strlen(s),s,0x0000,(void*)framebuffers[nflip]);
	flip(1);
	while(1);
}

void console_write(char *TEXT,u16 COULEUR)
{
	gp_drawString(2,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[0]);
	gp_drawString(2,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[1]);
	gp_drawString(2,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[2]);
	console_write_y+=11;
}

void console_write_no_lineup(char *TEXT,u16 COULEUR)
{
	gp_drawString(1,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[0]);
	gp_drawString(1,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[1]);
	gp_drawString(1,console_write_y,strlen(TEXT),TEXT,COULEUR,(u16*)framebuffers[2]);
}

u16* getCurrentBuffer()
{
	return (u16*)framebuffers[nflip];
}

void drawTextAt(char* text,int x,int y,u16 color)
{
	gp_drawString(x,y,strlen(text),text,color,(u16*)framebuffers[nflip]);
}
