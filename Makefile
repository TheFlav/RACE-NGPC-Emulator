CROSS_COMPILE = arm-open2x-linux-
SDL_BASE = arm-open2x-linux-
#LDFLAGS = -static
LDFLAGS = -s -static
STRIP = $(CROSS_COMPILE)strip

TARGET = race

# build tools
#GPP = g++
GPP = $(CROSS_COMPILE)g++
GCC = $(CROSS_COMPILE)gcc

#INCLUDE = -I. `sdl-config --cflags` `xml2-config --cflags`
#LIBS = `xml2-config --libs` `sdl-config --libs` -lSDL_image -lSDL_mixer

#CFLAGS = -g -DDRZ80 -DTARGET_GP2X -DNO_SOUND_OUTPUT -msoft-float `$(SDL_BASE)sdl-config --cflags` -O3 -Wall -ffast-math  -funroll-loops
CFLAGS = -DDRZ80 -DTARGET_GP2X -msoft-float `$(SDL_BASE)sdl-config --cflags` -O3 -Wall -ffast-math -funroll-loops -mcpu=arm920 -mtune=arm920t -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -fomit-frame-pointer -finline -finline-functions -fno-builtin -fno-common -mstructure-size-boundary=8 -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/arm-open2x-linux/include -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/arm-open2x-linux/include/SDL
#CFLAGS = -DDRZ80 -DTARGET_GP2X -msoft-float `$(SDL_BASE)sdl-config --cflags` -O3 -Wall -ffast-math  -funroll-loops
#CFLAGS = -g -DDRZ80 -DTARGET_GP2X -DNO_SOUND_OUTPUT -msoft-float `$(SDL_BASE)sdl-config --cflags`
LIBS = `$(SDL_BASE)sdl-config --libs` -L"/opt/open2x/gcc-4.1.1-glibc-2.3.6/arm-open2x-linux/lib" \
-lSDL -lSDL_gfx --start-group -lSDL_ttf -lfreetype \
-lSDL --end-group -lSDL_image -ljpeg -lpng12 -lz --start-group -lSDL_mixer -lvorbisidec -lmikmod -lsmpeg -lmad -lSDL --end-group -lgcc -lm -lc -lpthread -ldl $(LDFLAGS)

OBJS = gp2x.o \
	graphics.o \
	input.o \
	main.o \
	menu.o \
	memory.o \
	neopopsound.o \
	ngpBios.o \
	sound.o \
	tlcs900h.o \
	DrZ80_support.o DrZ80.o \
	memcpy.o \
	memset.o \
	flash.o \
	flush_uppermem_cache.o \
	state.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(GPP) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	$(RM) -f $(TARGET) $(COMPTARGET) $(OBJS)

.cpp.o:
	$(GPP) $(CFLAGS) -c $< -o $@

.c.o:
	$(GCC) $(CFLAGS) -c $< -o $@

.C.o:
	$(GCC) $(CFLAGS) -c $< -o $@

.s.o:
	$(GPP) $(CFLAGS) -c $< -o $@

.S.o:
	$(GPP) $(CFLAGS) -c $< -o $@


