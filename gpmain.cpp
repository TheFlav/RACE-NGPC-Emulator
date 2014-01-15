#include <stdio.h>
#include <stdlib.h>

#include "gp32.h"
#include "gfx.h"
#include "fileio.h"
#include "main.h"

#define memcpy gp2x_memcpy

extern "C" {

void gp2x_memcpy(void*,void*,unsigned int);
void EnableCache();
void gp_setCpuspeed(int);
void gp_disableIRQ();
void gp_enableIRQ();
void gp_installSWIIRQ(int i,void(*f)(void));
void gp_removeSWIIRQ(int i,void(*f)(void));
void gp_setMMU(unsigned char*,unsigned char*,unsigned short);

void gp_initSound(int,int,int);
unsigned short* gp_getRingSegment();
void gp_clearRingbuffer();
void GpPcmStop(void);

int gp_getButton();
void gp_Reset();

void gp_initRTC();
int gp_getRTC();
int gp_getPCLK();

void gp_drawString (int x,int y,int len,char *text,u16 color,void *framebuffer);

}

extern unsigned char _binary_intro_wav_start[];
#define INTRO_SIZE 219968
extern unsigned char _binary_splash_raw_start[];

#define LOOP 180

int turbo = 0;
int fskip = 2;
int autosave = 1;
int sound = 1;
volatile int intro = 0;
volatile int played = 44;

volatile u32 myGLOBALCOUNTER;

extern int audioCallback(unsigned short*,int);

static void MyTIMER4IRQ(void) __attribute__ ((interrupt ("IRQ")));

static void MyTIMER4IRQ(void)
{
	rTCON = BITCLEAR(rTCON,20); // timer4 off
	if (intro)
	{
		int len = LOOP<<2;
    	if (played>=INTRO_SIZE)
        	return;
		memcpy(gp_getRingSegment(),&_binary_intro_wav_start[played],
			   played+len>INTRO_SIZE?INTRO_SIZE-played:len);
	    played+=len;
	}
	else
		audioCallback(gp_getRingSegment(),LOOP);
	rTCON = BITSET(rTCON,20); // timer4 on
}

void gp_startSound()
{
	int pclk;
	if (sound)
	{
		pclk = gp_getPCLK();
		pclk/= 16;
		pclk/= 256;
		pclk/= 48;  // 51Hz timer, tested 33 66 133 Mhz

		gp_disableIRQ();
		rTCFG0 |= (0xFF<<8);   // Presacler for timer 2,3,4 = 256
		rTCFG1 |= (0x03<<16);  // timer4  1/16
		rTCNTB4 = (long)pclk;
		rTCON  = (0x1<<22) | (0x1<<20); // start timer4, auto reload
		gp_installSWIIRQ(14,MyTIMER4IRQ);
		gp_enableIRQ();
	}
}

//static 
void gp_stopSound()
{
	if (sound)
	{
		gp_disableIRQ();
		rTCON = BITCLEAR(rTCON,20); // timer4 off
		gp_removeSWIIRQ(14,MyTIMER4IRQ);
		gp_enableIRQ();
		gp_clearRingbuffer();
		GpPcmStop();
	}
}

int getTickCount() {
	return (gp_getRTC()*1000)>>6;
}

/*
 * ROM & Options selector
 */

DIR romlist;
int romcount;

int values[6] = {2,2,0,0,1,0};
const static char* options[8] = {"ROM","FrameSkip","CPU","Turbo mode","Sound","Save","Play !","Exit"};
const static int vmin[6] = {2,0,0,0,0,0};
const static int vcpu[15] = {133,144,156,160,164,166,
						 	 168,172,176,180,184,188,192,196,200};

void gp_mainMenu()
{
	const char*title = "RACE! Configuration menu";

	int vmax[6] = {romcount-1,5,14,1,1,1};

	u16* buf;
	int key,y=0,i,dy;
	char strbuf[32];

	gp_setCpuspeed(40);

	clearScreen();
	while(true)
	{
		buf = getCurrentBuffer();
		memset(buf,0,320*240*2);
		gp_drawString(50,20,strlen((char*)title),(char*)title,COLOR_WHITE,buf);
		for (i=0;i<8;i++)
		{
			dy = 66+i*(i<6?14:16);
			gp_drawString(i<6?50:130,dy,strlen((char*)options[i]),(char*)options[i],COLOR_WHITE,buf);
			switch(i)
			{
				case 0: //roms
					strcpy(strbuf,romlist.name[values[0]]);
					break;
				case 1:
					sprintf(strbuf,"%d",values[i]);
					break;
				case 2:
					sprintf(strbuf,"%d",vcpu[values[i]]);
					break;
				case 3:
				case 4:
				case 5:
					strcpy(strbuf,values[i]?"ON":"OFF");
					break;
				default:
					strbuf[0] = 0;
			}
			gp_drawString(180,dy,strlen(strbuf),strbuf,COLOR_WHITE,buf);
			if (y==i)
				gp_drawString(i<6?40:120,dy,1,">",COLOR_WHITE,buf);
		}
		flip(1);
		key=gp_getButton();
		if (key & (BUTTON_UP | BUTTON_LEFT | BUTTON_L))
			y = y==0 ? 7 : y-1;
		if (key & (BUTTON_DOWN | BUTTON_RIGHT | BUTTON_R))
			y = y==7 ? 0 : y+1;
		if (y==7 && (key & (BUTTON_A | BUTTON_B)))
			gp_Reset();
		if ((key & BUTTON_START) || (y==6 && (key & (BUTTON_A | BUTTON_B))))
			break;
		if (key & (BUTTON_A | BUTTON_SELECT))
			values[y] = (values[y]<vmax[y] ? values[y]+1 : vmin[y]);
		if (key & BUTTON_B)
			values[y] = (values[y]>vmin[y] ? values[y]-1 : vmax[y]);
		while ((key=gp_getButton())!=0);
	}
	while ((key=gp_getButton())!=0);
}

extern int mhemain(char*);

int main()
{
	int key;
	char romName[128];

	gp_initRTC();
	gp_setCpuspeed(66);
	initGfx();
	EnableCache();

	intro = 1;
	gp_initSound(8000,16,LOOP*8); 

	u16* buf = getCurrentBuffer();
	memcpy(buf,_binary_splash_raw_start,320*240*2);
	flip(1);

	gp_startSound();
	while(played<INTRO_SIZE)
	{
		key = gp_getButton();
		if ((key&BUTTON_A)||(key&BUTTON_B))
			break;
	}
	while (key=gp_getButton());
	gp_stopSound();
	intro = 0;

	romcount = smc_dir("dev0:\\GPMM\\NGPC\\",&romlist);

	while (true)
	{
		gp_mainMenu();
		turbo = values[3];
		sound = values[4];
		fskip = values[1];
		autosave = values[5];
		gp_setCpuspeed(vcpu[values[2]]);
		if (sound)
			gp_initSound(8000,16,LOOP*8); 
		sprintf(romName,"dev0:\\GPMM\\NGPC\\%s",romlist.name[values[0]]);
		mhemain(romName);
	}
}
