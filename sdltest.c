/* Found at http://friedspace.com/SDLTest.c */

#include <stdio.h>
#include <SDL.h>

#include "ffmpeg_test.h"

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixmem32;
	Uint32 colour;  
 
	colour = SDL_MapRGB( screen->format, r, g, b );
  
	pixmem32 = (Uint32*) screen->pixels  + y + x;
	*pixmem32 = colour;
}


void DrawScreen(SDL_Surface* screen, int h)
{ 
	int x, y, ytimesw;
  
	if(SDL_MUSTLOCK(screen)) 
	{
		if(SDL_LockSurface(screen) < 0) return;
	}

	for(y = 0; y < screen->h; y++ ) 
	{
		ytimesw = y*screen->pitch/BPP;
		for( x = 0; x < screen->w; x++ ) 
		{
			setpixel(screen, x, ytimesw, (x*x)/256+3*y+h, (y*y)/256+x+h, h);
		}
	}

	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
	SDL_Flip(screen); 
}


int sdl_main(int argc, char* argv[])
{
	SDL_Surface *screen;
	//SDL_Event event;

	//int keypress = 0;
	//int h=0; 

	if( SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 )
    {
        fprintf(stderr,"Failed to init SDL\n");
        return 1;
    }

	if( !(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)) )
	{
        fprintf(stderr,"Failed to set video mode\n");
		SDL_Quit();
		return 1;
	}

    if( !init_ffm( screen ) )
    {
        fprintf(stderr,"Failed to initialize FFM\n");
        SDL_Quit();
        return 1;
    }
    
	/*while(!keypress) 
	{
		DrawScreen(screen,h++);
		while(SDL_PollEvent(&event)) 
		{      
			switch (event.type) 
			{
				case SDL_QUIT:
					keypress = 1;
					break;
				case SDL_KEYDOWN:
					keypress = 1;
					break;
			}
		}
        }*/

    if( !dispatch_ffm() )
    {
        fprintf(stderr,"Failed to dispatch FFM\n");
    }
    
	SDL_Quit();

	return 0;
}
