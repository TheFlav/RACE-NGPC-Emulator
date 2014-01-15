// * Menu System for ngPsp * //
#ifndef _MENU_H_
#define _MENU_H_


// * Defines * //
//#define TARGETPATH "./"
//#define TARGETPATH "ms0:/PSP/GAME150/RACE-PSP/Roms/"
#define BGIMGPATH "./WALLPAPER.BMP"

#define ANALOG_MAX_TH	256 - 56
#define ANALOG_MIN_TH	56

#define MAX_FILE_LIST	200

extern SDL_Color white;// = {255,255,255,0};
extern SDL_Color ltGray;// = {255,255,255,0};
extern SDL_Color black;// = {0,0,0,0};
extern SDL_Color red;// = {255,0,0,0};
extern SDL_Color green;// = {255,0,0,0};
extern SDL_Color blue;// = {255,0,0,0};
extern SDL_Color yellow;// = {255,255,0,0};
#define TEXT_HEIGHT 8//12//15

//these need to fit into a byte
#define PSP_MHZ_222 22
#define PSP_MHZ_266 26
#define PSP_MHZ_333 33

#define NUM_OPTIONS 5
#define TURBO_OPTION 0
#define HICOLOR_OPTION 1
#define FPS_OPTION 2
#define PERIODIC_SAVES_OPTION 3
#define PSP_MHZ_OPTION 4
extern Uint8 options[NUM_OPTIONS];
extern char optionNames[NUM_OPTIONS][100];


// * Functions * //
void HandleMenu( void );
void DrawMenu( void );
int LoadRomFromGP2X( char *fileName, SDL_Surface* screen);
void printTTF(char *msg, int x, int y, SDL_Color clrFg, int updateScreen, SDL_Surface* screen, int fillBlack);
//int BuildFileList( dirent_t* pFileList );
int DisplayWarning( void );
int strrchr2(const char *src, int c);

// * Varibles * //
extern int iMenuSelect;
extern int iClockSpeed;

// * External Varibles (defined outside of menu.c) * //
/*extern int bEmulatorRunning;
extern int iScreenInfo[3];
extern pspCtrlData myInput;
extern unsigned char rom_image[8*1024*1024];
extern unsigned short uBuffer[480*272];
extern int ifpsFrame;
*/
// * EoF * //
#endif
