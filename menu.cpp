#include <dirent.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "menu.h"
#include "StdAfx.h"
#include "graphics.h"
#include "GP2X.h"

extern "C" {
void gp2x_memcpy(void*,void*,unsigned int);
}


extern int exitNow;

SDL_Color white = {255,255,255,0};
SDL_Color ltGray = {200,200,200,0};
SDL_Color black = {0,0,0,0};
SDL_Color red = {255,0,0,0};
SDL_Color green = {0,255,0,0};
SDL_Color blue = {0,0,255,0};
SDL_Color yellow = {255,255,0,0};

#define DISPLAY_NUM_ROMS 17 //16 for 480x272
#define DISPLAY_OFFSET 8

void printTTF(char *msg, int x, int y, SDL_Color clrFg, int updateScreen, SDL_Surface* pScreen, int fillBlack)
{
	static TTF_Font *fnt=NULL;
	static int startup = 1;

    if(pScreen==NULL)
        return;

	if(startup)
	{
		fnt = TTF_OpenFont("font.ttf", TEXT_HEIGHT);
        if(fnt==NULL)
            printf("font.ttf not found.  Text output failure.\n");
        startup = 0;
	}

	if(fnt==NULL)
	{
        return;
    }

	SDL_Surface *sTemp = TTF_RenderText_Solid(fnt, msg, clrFg);

	SDL_Rect rcDest = {x,y,sTemp->w,sTemp->h};

    // Put it on the surface (sText):
	if(fillBlack)
		SDL_FillRect(pScreen, &rcDest, SDL_MapRGB(pScreen->format,0,0,0));//fill black
	SDL_BlitSurface(sTemp, NULL, pScreen, &rcDest);

	SDL_FreeSurface(sTemp);

	if(updateScreen)
        SDL_UpdateRects (pScreen, 1, &rcDest);
}

int strrchr2(const char *src, int c)
{
	size_t len;

	len=strlen(src);
	while(len>0){
		len--;
		if(*(src+len) == c)
			return len;
	}

	return 0;
}

void reset(void)
{
}

void rom_unload(void)
{
}

int DebugLoop( char* szString, unsigned long uColor )
{
}


//* -------------------------------------------------------------------------------------

int iMenuSelect = 0;
int iClockSpeed = -1;

//* -------------------------------------------------------------------------------------

#define O_RDONLY    0x0001
#define O_WRONLY    0x0002
#define O_RDWR      0x0003
#define O_NBLOCK    0x0010
#define O_APPEND    0x0100
#define O_CREAT     0x0200
#define O_TRUNC     0x0400
#define O_NOWAIT    0x8000


#define SZFP_LEN 256
char szFolderPath[SZFP_LEN]={0};

typedef struct dirent SceIoDirent;


int BuildFileList(SceIoDirent* pFileList )
{
	int iFileCount = 0;
    DIR *dirPtr;
    SceIoDirent *entry;

	dirPtr = opendir( szFolderPath );

	if(!dirPtr)
		return -1;

	while( iFileCount < MAX_FILE_LIST )
	{
	    entry = readdir(dirPtr);
		if( entry == NULL )  // End of List, Exit Loop
			break;

		if(entry->d_type == DT_REG) //is it a file?
		{
			int iDepth = 0;

			iDepth = strrchr2( entry->d_name, '.' );

			iDepth++;

			if( ( strcmp( entry->d_name + iDepth, "ngp" ) == 0 ) || ( strcmp( entry->d_name + iDepth, "NGP" ) == 0 ) || ( strcmp( entry->d_name + iDepth, "ngc" ) == 0 ) || ( strcmp( entry->d_name + iDepth, "NGC" ) == 0 )  || ( strcmp( entry->d_name + iDepth, "npc" ) == 0 ) || ( strcmp( entry->d_name + iDepth, "NPC" ) == 0 )  || ( strcmp( entry->d_name + iDepth, "zip" ) == 0 ) || ( strcmp( entry->d_name + iDepth, "ZIP" ) == 0 ))
			{
			    memcpy(&pFileList[iFileCount], entry, sizeof(SceIoDirent));
				iFileCount++;
			}
		}
		else //a directory?
		{
			if(strcmp(entry->d_name,".")!=0)  //don't do the "."
            {
			    memcpy(&pFileList[iFileCount], entry, sizeof(SceIoDirent));
                iFileCount++;
            }
		}
	}

	closedir( dirPtr );

	return iFileCount;
}

//* -------------------------------------------------------------------------------------

void rom_loaded(void)
{
/*	int i;

	//Extract the header
	rom_header = (RomHeader*)(rom.data);

	//Rom Name
	for(i = 0; i < 12; i++)
	{
		if (rom_header->name[i] >= 32 && rom_header->name[i] < 128)
			rom.name[i] = rom_header->name[i];
		else
			rom.name[i] = ' ';
	}
	rom.name[i] = 0;

	rom_hack();	//Apply a hack if reuqired!

	rom_display_header();	//Show the header (debugger only)

	flash_read();*/
}


int compareDirents( const void *arg1, const void *arg2 )
{
	SceIoDirent* d1 = (SceIoDirent*)arg1;
	SceIoDirent* d2 = (SceIoDirent*)arg2;
	int d1IsFile = d1->d_type == DT_REG;
	int d2IsFile = d2->d_type == DT_REG;

    if(d1IsFile && d2IsFile)
        return strcmp(d1->d_name, d2->d_name);//was stricmp
    if(!d1IsFile && !d2IsFile)
        return strcmp(d1->d_name, d2->d_name);//was stricmp
    if(d1IsFile && !d2IsFile)
        return 1;
    return -1;
}

#define DOWN(x)	keystates[x]

Uint8 options[NUM_OPTIONS];
char optionNames[NUM_OPTIONS][100] =
{
    "\"TURBO\" Speedup Hack",
    "HiColor Support (slower)",
    "Show Frames Per Second Counter",
    "Write Savegame File Periodically",
    "PSP Clock Frequency"
};
const Uint8 defaultOptions[NUM_OPTIONS] =
{
    1,//"\"TURBO\" Speedup Hack"
    0, //"HiColor Support"
    0,  //FPS
    1,  //periodic saves
    PSP_MHZ_333 //333 MHz
};

#define OPTIONS_FILE "raceOptions.bin"

int saveOptions()
{
    FILE *fd = NULL;
    fd = fopen(OPTIONS_FILE, "wb");
    if(!fd)
        return (0);
    fwrite(options, 1, sizeof(options), fd);
    fwrite(szFolderPath, 1, SZFP_LEN,fd);
    fclose(fd);
    return 1;
}

int loadOptions()
{
    FILE *fd = NULL;
    fd = fopen(OPTIONS_FILE, "rb");
    if(!fd)
        return (0);
    fread(options, 1, sizeof(options), fd);
    fread(szFolderPath, 1, SZFP_LEN,fd);
    fclose(fd);
    return 1;
}

int initOptions()
{
    int i;

    //first, try to read options struct from a file
    if(loadOptions())
        return 1;

    //if that doesn't work, then just load defaults
    for(i=0;i<NUM_OPTIONS;i++)
        options[i] = defaultOptions[i];

    if(szFolderPath[0] == 0)
    {
        //set initial szFolderPath
        getcwd(szFolderPath,SZFP_LEN);
        strcat( szFolderPath, "/" );
    }

    return 2;
}



int doOptionsMenu(SDL_Surface* screen)
{
    Uint8 *keystates;
	static int iSelectOffset = 0;
	static int iScrollOffset = 0;
	int i;

	int iWaitLoop = 0; //* Because I am bored waiting scrolling

	int iOneTimeUseInput = 0;

    iSelectOffset = 0;
    iScrollOffset = 0;
    iOneTimeUseInput = 1;

    do
    {
        GP2X_SetKeyStates();
    }while(GP2X_anybuttondown());

	while(!exitNow)
	{
		// * Display File Names * //
        if(iOneTimeUseInput)
        {
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));//fill black
#ifdef TARGET_PSP
            printTTF("RACE! PRESS X TO CHANGE OPTION (O to save/exit)", 0, TEXT_HEIGHT*0, yellow, 0, screen, 0);
#else
            printTTF("RACE! PRESS A TO CHANGE OPTION (B to save/exit)", 0, TEXT_HEIGHT*0, yellow, 0, screen, 0);
#endif
            printTTF("http://cfc2english.blogspot.com", 0, TEXT_HEIGHT*18, yellow, 0, screen, 0);

            for( i = iScrollOffset; ( i < (iScrollOffset + DISPLAY_NUM_ROMS) ) && ( i < NUM_OPTIONS ); i++ )
            {
                char optionStr[100];

                switch(i)
                {
                    case PSP_MHZ_OPTION:
                        switch(options[i])
                        {
                            case PSP_MHZ_222:
                                sprintf(optionStr, "%s is 222", optionNames[i]);
                                break;
                            case PSP_MHZ_266:
                                sprintf(optionStr, "%s is 266", optionNames[i]);
                                break;
                            default:
                                //options[i]=PSP_MHZ_333;
                            case PSP_MHZ_333:
                                sprintf(optionStr, "%s is 333", optionNames[i]);
                                break;
                        }
                        break;
                    default:
                        sprintf(optionStr, "%s is %s", optionNames[i], options[i]?"ON":"OFF");
                        break;
                }
                if( i == iSelectOffset )
                    printTTF(optionStr, 5, TEXT_HEIGHT*(1+i-iScrollOffset), red, 0, screen, 0);
                else
                    printTTF(optionStr, 10, TEXT_HEIGHT*(1+i-iScrollOffset), white, 0, screen, 0);
            }

            SDL_Flip(screen);
        }

		// * Input For Selection * //
        GP2X_SetKeyStates();
        keystates = GP2X_GetKeyStateArray(NULL);

		if(!GP2X_anybuttondown())
		{
			iOneTimeUseInput = 0;
			iWaitLoop = 0;
		}

		if( iWaitLoop++ > 50 )
			iOneTimeUseInput = 0;

		if( (DOWN(SDLK_b)) && !iOneTimeUseInput )
		{
            do
            {
                GP2X_SetKeyStates();
            }while(GP2X_anybuttondown());

            return -1;
		}

		if( (DOWN(SDLK_a)) && !iOneTimeUseInput )
		{
		    //we need to change the option here

            switch(iSelectOffset)
            {
                case PSP_MHZ_OPTION:
                {
                    Uint8 newMHz=PSP_MHZ_333;
                    switch(options[iSelectOffset])
                    {
                        case PSP_MHZ_222:
                            newMHz=PSP_MHZ_266;
                            break;
                        default:
                        case PSP_MHZ_266:
                            newMHz=PSP_MHZ_333;
                            break;
                        case PSP_MHZ_333:
                            newMHz=PSP_MHZ_222;
                            break;
                    }
                    options[iSelectOffset] = newMHz;
                    break;
                }
                default:
                    options[iSelectOffset] = !options[iSelectOffset];
                    break;
            }

            do
            {
                GP2X_SetKeyStates();
            }while(GP2X_anybuttondown());
			iOneTimeUseInput = 1;
		}

		if( (DOWN(SDLK_UP)) && !iOneTimeUseInput && iSelectOffset )
			--iSelectOffset;

		if( ( DOWN(SDLK_DOWN) ) && !iOneTimeUseInput && ( iSelectOffset < NUM_OPTIONS - 1 ) )
			++iSelectOffset;

		if( GP2X_anybuttondown() )
		{
			iOneTimeUseInput = 1;
		}


		// * Update Scroll Of Text * //

		iScrollOffset = iSelectOffset - DISPLAY_OFFSET;

		if( iScrollOffset + DISPLAY_NUM_ROMS > NUM_OPTIONS )
			iScrollOffset = NUM_OPTIONS - DISPLAY_NUM_ROMS;

		if( iScrollOffset < 0 )
			iScrollOffset = 0;
	}

	return 1;
}

int LoadRomFromGP2X( char *fileName, SDL_Surface* screen)
{
	static SceIoDirent FileList[MAX_FILE_LIST];
    Uint8 *keystates;
	int iFileCount = 0;
	static int iSelectOffset = 0;
	static int iScrollOffset = 0;
	int i;

	int iWaitLoop = 0; //* Because I am bored waiting scrolling

	int iOneTimeUseInput = 0;

	initOptions();  //set up the options struct

rescanDirectory:

    do
    {
        GP2X_SetKeyStates();
    }while(GP2X_anybuttondown());

	iFileCount = BuildFileList( &FileList[0] );

	if( iFileCount <= 0 )  //this should only happen if szFolderPath is invalid
	{
        strcpy(szFolderPath,"ms0:/");//panic and start at the root
        goto rescanDirectory;
	}

	//if( iSelectOffset > iFileCount )
	{
		iSelectOffset = 0;
		iScrollOffset = 0;
	}

    iOneTimeUseInput = 1;

	//sort the file list
	qsort((void *)FileList, (size_t)iFileCount, sizeof( SceIoDirent ), compareDirents);

	while(!exitNow)
	{
		// * Display File Names * //
        if(iOneTimeUseInput)
        {
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,0,0,0));//fill black
#ifdef TARGET_PSP
            printTTF("RACE! PRESS X TO LOAD GAME FILE (O for Options)", 0, TEXT_HEIGHT*0, yellow, 0, screen, 0);
#else
            printTTF("RACE! PRESS A TO LOAD GAME FILE (B for Options)", 0, TEXT_HEIGHT*0, yellow, 0, screen, 0);
#endif

            printTTF(szFolderPath, 0, TEXT_HEIGHT*18, yellow, 0, screen, 0);
//            printTTF("http://cfc2english.blogspot.com", 0, TEXT_HEIGHT*18, yellow, 0, screen, 0);

            for( i = iScrollOffset; ( i < (iScrollOffset + DISPLAY_NUM_ROMS) ) && ( i < iFileCount ); i++ )
            {
                if( i == iSelectOffset )
                    printTTF(FileList[i].d_name, 5, TEXT_HEIGHT*(1+i-iScrollOffset), red, 0, screen, 0);
                else
                {
                    if(FileList[i].d_type == DT_REG)//a file
                        printTTF(FileList[i].d_name, 10, TEXT_HEIGHT*(1+i-iScrollOffset), white, 0, screen, 0);
                    else
                        printTTF(FileList[i].d_name, 10, TEXT_HEIGHT*(1+i-iScrollOffset), ltGray, 0, screen, 0);
                }
            }

            SDL_Flip(screen);
        }

		// * Input For Selection * //
        GP2X_SetKeyStates();
        keystates = GP2X_GetKeyStateArray(NULL);

		if(!GP2X_anybuttondown())
		{
			iOneTimeUseInput = 0;
			iWaitLoop = 0;
		}

		if( iWaitLoop++ > 50 )
			iOneTimeUseInput = 0;

		if( (DOWN(SDLK_UP)) && !iOneTimeUseInput && iSelectOffset )
			--iSelectOffset;

		if( ( DOWN(SDLK_DOWN) ) && !iOneTimeUseInput && ( iSelectOffset < iFileCount - 1 ) )
			++iSelectOffset;

		if( (DOWN(SDLK_b)) && !iOneTimeUseInput )
		{
			doOptionsMenu(screen);
			saveOptions();
			iOneTimeUseInput = 1;
		}

#ifdef TARGET_WIN
		if( (DOWN(SDLK_q)) && !iOneTimeUseInput )
		{
			return 0;
		}

#endif

		if( (DOWN(SDLK_a)) && !iOneTimeUseInput )
		{
		    if(FileList[iSelectOffset].d_type == DT_REG)
		    {
		        //it's a file, so load it
                strcpy(fileName, szFolderPath);
                //strcat(fileName, "Roms/" );
                strcat(fileName, FileList[iSelectOffset].d_name );
		    }
		    else
		    {
		        if(strcmp(FileList[iSelectOffset].d_name, "..")==0)//up a directory
		        {
		            //pull off a dir
                    int iDepth = 0;

                    iDepth = strrchr2(szFolderPath, '/' );
                    szFolderPath[iDepth]=0;
                    iDepth = strrchr2(szFolderPath, '/' );
                    szFolderPath[iDepth+1]=0;
		        }
		        else
		        {
		            //add a dir
		            strcat(szFolderPath, FileList[iSelectOffset].d_name);
		            strcat(szFolderPath, "/");
		        }
                goto rescanDirectory;
		    }

            saveOptions();
			return 1;
		}

		if( GP2X_anybuttondown() )
		{
			iOneTimeUseInput = 1;
		}


		// * Update Scroll Of Text * //

		iScrollOffset = iSelectOffset - DISPLAY_OFFSET;

		if( iScrollOffset + DISPLAY_NUM_ROMS > iFileCount )
			iScrollOffset = iFileCount - DISPLAY_NUM_ROMS;

		if( iScrollOffset < 0 )
			iScrollOffset = 0;
	}
	return 0;
}

