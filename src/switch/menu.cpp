#include <dirent.h>

#include <vector>
//#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <algorithm>
//#include <fstream>

#include <iostream>
#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
using namespace std;

static SDL_Surface *windowSurface = NULL;
//static const char *temp = "sdmc:/Switch/desmume/roms/Paint.nds";
static const char *path = "sdmc:/Switch/desmume/roms/";
string listedPath = "";
TTF_Font *fonta;
static int cursor = 0;
static vector<string> entries;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect bUI;
SDL_Rect tUI;

void DrawText(TTF_Font *font, int x, int y, SDL_Color colour, const char *text)
{
	SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, colour, 1280);
	SDL_SetSurfaceAlphaMod(surface, colour.a);
	SDL_Rect position = {x, y, surface->w, surface->h};
	SDL_BlitSurface(surface, NULL, windowSurface, &position);
	SDL_FreeSurface(surface);
}

void Selector(){
	SDL_SetRenderDrawColor(renderer, 88, 88, 88, 255);
	SDL_RenderClear(renderer);
	//int TY = 120;
	int TY = 60;
	for(int i = 0; i < entries.size(); i++)
	{
		if(i >= cursor){
			if(TY <= 540)
				TY += 30;
		} else if (i < cursor){
		}
		string ext3 = entries[i].substr(entries[i].length() - 3);
		transform(ext3.begin(), ext3.end(), ext3.begin(), ::tolower);
		if(ext3 == "nds" || ext3 == "srl")
		{
			if(i == cursor) DrawText(fonta, 30, TY, {128, 128, 255, 255}, entries[i].c_str());
			else DrawText(fonta, 30, TY, {255, 255, 255, 255}, entries[i].c_str());
		}
		else
		{
			if(i == cursor) DrawText(fonta, 30, TY, {128, 128, 255, 180}, entries[i].c_str());
			else DrawText(fonta, 30, TY, {255, 255, 255, 180}, entries[i].c_str());
		}
		//TY += 30;
	}
	SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
	bUI.x = 0;
	bUI.y = 570;
	bUI.w = 1280;
	bUI.h = 150;
	SDL_RenderFillRect( renderer, &bUI );
	tUI.x = 0;
	tUI.y = 0;
	tUI.w = 1280;
	tUI.h = 90;
	SDL_RenderFillRect( renderer, &tUI );
	DrawText(fonta, 30, 30, {255, 255, 255, 255}, "DeSmuME-NX - La Crude Selector");
	DrawText(fonta, 1030, 30, {255, 255, 255, 255}, "- Created by Laprox");
	DrawText(fonta, 1060, 660, {255, 255, 255, 255}, "(A) Load ROM");
	DrawText(fonta, 1040, 600, {255, 255, 255, 255}, "(^) (v) Navigate");
	string vert = "Vertical Mod: Press (B) to " + string(UserConfiguration.portraitEnabled? "disable" : "enable");
	DrawText(fonta, 30, 600, {255, 255, 255, 255}, vert.c_str());
	string fs = "Frame Skip Value: Press (<) or (>) to decrease or increase: " + to_string(UserConfiguration.frameSkip);
	DrawText(fonta, 30, 630, {255, 255, 255, 255}, fs.c_str());
	string snd = "Sound: Press (X) to " + string(UserConfiguration.soundEnabled? "disable" : "enable");
	DrawText(fonta, 30, 660, {255, 255, 255, 255}, snd.c_str());
	SDL_RenderPresent(renderer);
}

void Explore(const char *dir_name){
	DIR *dir;
	entries.clear();
	struct dirent *entry;
	
	dir = opendir(dir_name);
	if(!dir){
		DrawText(fonta, 500, 30, {255, 255, 255, 255}, "Error: Directory not found!");
		return;
	}
	while(true){
		entry = readdir(dir);
		if(entry == NULL) break;
		listedPath = string(dir_name) + string(entry->d_name);
		entries.push_back(listedPath);
		
	}
	closedir(dir);
	Selector();
}

const char* menu_FileBrowser() 
{
	romfsInit();
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	fonta=TTF_OpenFont("romfs:/Roboto-Regular.ttf", 25);
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		std::cout << "Failed to initialise! Error: " << SDL_GetError( ) << std::endl;
	}
	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if ( NULL == window )
    {
        std::cout << "Failed to create window! Error: " << SDL_GetError( ) << std::endl;
        return NULL;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	windowSurface = SDL_GetWindowSurface( window );
	Explore(path);
	char *name = (char*)malloc(4096);

	while(true){
		hidScanInput();
		int keysDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if(keysDown & KEY_A)
		{
			if(cursor < entries.size())
			{
				name = const_cast<char*>(entries[cursor].c_str());
				break;
			}
		}
		else if(keysDown & KEY_B)
		{
			UserConfiguration.portraitEnabled = !UserConfiguration.portraitEnabled;
			Selector();
		}
		else if(keysDown & KEY_X)
		{
			UserConfiguration.soundEnabled = !UserConfiguration.soundEnabled;
			Selector();
		}
		else if(keysDown & KEY_DOWN)
		{
			cursor++;
			if(cursor > (entries.size() - 1)) cursor = entries.size() - 1;
			Selector();
		}
		else if(keysDown & KEY_UP)
		{
			cursor--;
			if(cursor < 0) cursor = 0;
			Selector();
		}
		else if(keysDown & KEY_LEFT)
		{
			if(UserConfiguration.frameSkip > 0)
			UserConfiguration.frameSkip--;
			Selector();
		}

		else if(keysDown & KEY_RIGHT)
		{
			UserConfiguration.frameSkip++;
			Selector();
		}
		SDL_UpdateWindowSurface( window );

	}
	//FILE *file;
	//file = fopen("sdmc:/Switch/desmume/log.txt", "w");
	//fwrite(name/*.c_str()*/, 3, 100, file);
	//fclose(file);
	
    TTF_CloseFont(fonta);
	fonta=NULL;	
    SDL_FreeSurface( windowSurface );
	windowSurface = NULL;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
    romfsExit();
	return name;
}