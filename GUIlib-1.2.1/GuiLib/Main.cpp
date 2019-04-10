#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "GUI.h"
#include "GUI_widgets.h"
#include "GUI_output.h"

#include "SDL_syswm.h"

// Variables that we'll need to clean up 
GUI *gui;
HWND handle;
int ScreenWidth = 640;
int ScreenHeight = 350;

enum image_names 
{
	IMAGE_START,
	IMAGE_EXIT,
	IMAGE_BACKGROUND,
	NUM_IMAGES
};

char *image_files[NUM_IMAGES] = 
{
	"start.bmp", 
	"exit.bmp", 
	"background.bmp"
};

SDL_Surface *images[NUM_IMAGES];

void cleanup(void)
{
	int i;
	delete gui;
	for ( i=0; i<NUM_IMAGES; ++i ) 
	{
		if ( images[i] ) 
		{
			SDL_FreeSurface(images[i]);
		}
	}
}

void centerWindow()
{
	//Retrieve the monitor resolution size. (AKA the size of the desktop in pixels)
	RECT rect;
	GetClientRect(GetDesktopWindow(),&rect);
	int monitorWidth = rect.right;
	int monitorHeight = rect.bottom;

	//Calculate the new window position.
	//'Screen' in this case refers to the window size, and not the monitor/desktop size.
	int posX = (monitorWidth - ScreenWidth) / 2;
	int posY = (monitorHeight - ScreenHeight) / 2;

	//Grab a handle to the window.
	SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);

	if(SDL_GetWMInfo(&windowInfo))
	{
		handle = windowInfo.window;
	
		//Finally, move the window to the calculated position, using the WIN32 function 'SetWindowPos'.
		if(SetWindowPos(handle, NULL, posX, posY, 0, 0, SWP_NOREPOSITION|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE) == FALSE)
		{
			printf("Error occured centering window on the screen using the WIN32 function SetWindowPos()");
		}
	}
	else
	{
		printf("SDL_GetWMInfo(): %s", SDL_GetError());
	}
}

void animateClose()
{
	//bool test = AnimateWindow(handle, 2000, AW_HIDE | AW_BLEND);
}

void loadEngine()
{
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	int error;
	SDL_Rect null_rect = { 0, 0, 0, 0 };

	// Initialize SDL 
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	// Get a video mode for display
	screen = SDL_SetVideoMode(ScreenWidth, ScreenHeight, 16, SDL_SWSURFACE | SDL_NOFRAME);
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Couldn't set video mode: %s\n",SDL_GetError());
		exit(1);
	}
	centerWindow();

	// Create a GUI container
	gui = new GUI(screen);

	/* Load our images */
	for (int i = 0; i < NUM_IMAGES; ++i ) 
	{
		images[i] = NULL;
	}

	error = 0;
	for (int i = 0; i < NUM_IMAGES; ++i ) 
	{
		images[i] = SDL_LoadBMP(image_files[i]);
		if ( images[i] == NULL ) 
		{
			fprintf(stderr, "Couldn't load '%s': %s\n", image_files[i], SDL_GetError());
			++error;
		}
	}
	if ( error ) 
	{
		cleanup();
		exit(1);
	}

	// Add background
	GUI_Image* bg = new GUI_Image(0, 0, images[IMAGE_BACKGROUND]);
	gui->AddWidget(bg);

	// Add our interface widgets:
	// We want a gray background and a centered image button.
	GUI_Widget* cmdStart = new GUI_Button(NULL, 50, 50, images[IMAGE_START], images[IMAGE_START]);
	gui->AddWidget(cmdStart);

	GUI_Widget* cmdExit = new GUI_Button(NULL, 50, 100, images[IMAGE_EXIT], images[IMAGE_EXIT]);
	gui->AddWidget(cmdExit);

	// Run the GUI, and then clean up when it's done.
	gui->Run(NULL);

	loadEngine();

	cleanup();
	exit(0);

	return(0);
}
