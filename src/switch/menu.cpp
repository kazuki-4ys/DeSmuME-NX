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
#include <SDL2/SDL2_rotozoom.h>

#include "config.h"
using namespace std;

static SDL_Surface *windowSurface = NULL;
//static const char *temp = "sdmc:/Switch/desmume/roms/Paint.nds";
static const char *path = "sdmc:/Switch/desmume/roms/";
string listedPath = "";
TTF_Font *fontA;
TTF_Font *fontB;
static int cursor = 0;
static vector<string> entries;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect bgUI;
SDL_Rect bUI;
SDL_Rect tUI;
Uint16 A[] = {0xe0a0};
Uint16 B[] = {0xe0a1};
Uint16 X[] = {0xe0a2};
Uint16 UPDOWN[] = {0xe0ce};
Uint16 LEFTRIGHT[] = {0xe0cb};
//SDL_Texture *texture;
//SDL_Point topLeft = {0, 0};

void DrawText(TTF_Font *font, int x, int y, SDL_Color colour, const char *text)
{
	SDL_Surface *surface = UserConfiguration.portraitEnabled? TTF_RenderText_Blended(font, text, colour) : TTF_RenderText_Blended_Wrapped(font, text, colour, 1280);
	SDL_SetSurfaceAlphaMod(surface, colour.a);
	//if(!UserConfiguration.portraitEnabled){
		SDL_Rect position = {x, y, surface->w, surface->h};
		SDL_BlitSurface(surface, NULL, windowSurface, &position);
	/*} else{
		SDL_Rect position = {x, y, surface->w, surface->h};
		//SDL_Rect position = {y, x, surface->w, surface->h};
		//SDL_Surface* rot = rotozoomSurface(surface, 90, 1.0, 0);
		//SDL_BlitSurface(rot, NULL, windowSurface, &position);
		SDL_BlitSurface(surface, NULL, windowSurface, &position);
		SDL_Surface* rot = rotozoomSurface(windowSurface, 90, 1.0, 0);
		SDL_BlitSurface(rot, NULL, windowSurface, &position);
		SDL_FreeSurface(rot);
	}*/
	SDL_FreeSurface(surface);
}

void DrawSymbol(TTF_Font *font, int x, int y, SDL_Color colour, const Uint16 *text)
{
	SDL_Surface *surface = UserConfiguration.portraitEnabled? TTF_RenderUNICODE_Blended(font, text, colour) : TTF_RenderUNICODE_Blended_Wrapped(font, text, colour, 1280);
	SDL_SetSurfaceAlphaMod(surface, colour.a);
	//if(!UserConfiguration.portraitEnabled){
		SDL_Rect position = {x, y, surface->w, surface->h};
		SDL_BlitSurface(surface, NULL, windowSurface, &position);
	/*} else{
		SDL_Rect position = {x, y, surface->w, surface->h};
		//SDL_Rect position = {y, x, surface->w, surface->h};
		//SDL_Surface* rot = rotozoomSurface(surface, 90, 1.0, 0);
		//SDL_BlitSurface(rot, NULL, windowSurface, &position);
		SDL_BlitSurface(surface, NULL, windowSurface, &position);
		SDL_Surface* rot = rotozoomSurface(windowSurface, 90, 1.0, 0);
		SDL_BlitSurface(rot, NULL, windowSurface, &position);
		SDL_FreeSurface(rot);
	}*/
	SDL_FreeSurface(surface);
}

void Selector(){
	SDL_SetRenderDrawColor(renderer, 88, 88, 88, 255);
	bgUI.x = 0;
	bgUI.y = 0;
	bgUI.w = 1280;
	bgUI.h = 1280;
	SDL_RenderFillRect( renderer, &bgUI );
	SDL_RenderClear(renderer);
	//int TY = 120;
	int TY = 60;
	if(!UserConfiguration.portraitEnabled){
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
				if(i == cursor) DrawText(fontA, 30, TY, {128, 128, 255, 255}, entries[i].c_str());
				else DrawText(fontA, 30, TY, {255, 255, 255, 255}, entries[i].c_str());
			}
			else
			{
				if(i == cursor) DrawText(fontA, 30, TY, {128, 128, 255, 180}, entries[i].c_str());
				else DrawText(fontA, 30, TY, {255, 255, 255, 180}, entries[i].c_str());
			}
		//TY += 30;
		}
	} else{
		for(int i = 0; i < entries.size(); i++)
		{
			if(i >= cursor){
				if(TY <= 1190)
				TY += 30;
			} else if (i < cursor){
			}
			string ext3 = entries[i].substr(entries[i].length() - 3);
			transform(ext3.begin(), ext3.end(), ext3.begin(), ::tolower);
			if(ext3 == "nds" || ext3 == "srl")
			{
				if(i == cursor) DrawText(fontA, 30, TY, {128, 128, 255, 255}, entries[i].c_str());
				else DrawText(fontA, 30, TY, {255, 255, 255, 255}, entries[i].c_str());
			}
			else
			{
				if(i == cursor) DrawText(fontA, 30, TY, {128, 128, 255, 180}, entries[i].c_str());
				else DrawText(fontA, 30, TY, {255, 255, 255, 180}, entries[i].c_str());
			}
		}
	}
	SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255);
	/*bUI.x = UserConfiguration.portraitEnabled? 1130 : 0;
	bUI.y = UserConfiguration.portraitEnabled? 0 : 570;
	bUI.w = UserConfiguration.portraitEnabled? 150 : 1280;
	bUI.h = UserConfiguration.portraitEnabled? 720 : 150;
	SDL_RenderFillRect( renderer, &bUI );
	tUI.x = UserConfiguration.portraitEnabled? 0 : 0;
	tUI.y = UserConfiguration.portraitEnabled? 0 : 0;
	tUI.w = UserConfiguration.portraitEnabled? 90 : 1280;
	tUI.h = UserConfiguration.portraitEnabled? 720 : 90;
	SDL_RenderFillRect( renderer, &tUI );*/
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
	//if(!UserConfiguration.portraitEnabled){
		DrawText(fontA, 30, 30, {255, 255, 255, 255}, "DeSmuME-NX - La Crude Selector");
		DrawText(fontA, 1000, 30, {255, 255, 255, 255}, "- Created by Laprox");
		DrawSymbol(fontB, 1060, 660, {255, 255, 255, 255}, A);
		DrawText(fontA, 1090, 660, {255, 255, 255, 255}, " Load ROM");
		DrawSymbol(fontB, 1060, 600, {255, 255, 255, 255}, UPDOWN);
		DrawText(fontA, 1090, 600, {255, 255, 255, 255}, " Navigate");
		string vert = "Vertical Mod: Press      to " + string(UserConfiguration.portraitEnabled? "disable" : "enable");
		DrawSymbol(fontB, 250, 600, {255, 255, 255, 255}, B);
		DrawText(fontA, 30, 600, {255, 255, 255, 255}, vert.c_str());
		string fs = "Frame Skip Value: Press      to decrease or increase: " + to_string(UserConfiguration.frameSkip);
		DrawSymbol(fontB, 305, 630, {255, 255, 255, 255}, LEFTRIGHT);
		DrawText(fontA, 30, 630, {255, 255, 255, 255}, fs.c_str());
		string snd = "Sound: Press       to " + string(UserConfiguration.soundEnabled? "disable" : "enable");
		DrawSymbol(fontB, 185, 660, {255, 255, 255, 255}, X);
		DrawText(fontA, 30, 660, {255, 255, 255, 255}, snd.c_str());
	/*} else{
		DrawText(fontA, 30, 400, {255, 255, 255, 255}, "La Crude Selector");
		DrawText(fontA, 30, 30, {255, 255, 255, 255}, "- Created by Laprox");
		DrawSymbol(fontB, 1250, 120, {255, 255, 255, 255}, A);
		DrawText(fontA, 1250, 90, {255, 255, 255, 255}, " Load ROM");
		DrawSymbol(fontB, 1060, 600, {255, 255, 255, 255}, UPDOWN);
		DrawText(fontA, 1090, 600, {255, 255, 255, 255}, " Navigate");
		string vert = "Vertical Mod: Press     to " + string(UserConfiguration.portraitEnabled? "disable" : "enable");
		DrawSymbol(fontB, 285, 600, {255, 255, 255, 255}, B);
		DrawText(fontA, 30, 600, {255, 255, 255, 255}, vert.c_str());
		string fs = "Frame Skip Value: Press     to decrease or increase: " + to_string(UserConfiguration.frameSkip);
		DrawSymbol(fontB, 340, 630, {255, 255, 255, 255}, LEFTRIGHT);
		DrawText(fontA, 30, 630, {255, 255, 255, 255}, fs.c_str());
		string snd = "Sound: Press     to " + string(UserConfiguration.soundEnabled? "disable" : "enable");
		DrawSymbol(fontB, 200, 660, {255, 255, 255, 255}, X);
		DrawText(fontA, 30, 660, {255, 255, 255, 255}, snd.c_str());*/
		/*if(UserConfiguration.portraitEnabled){//Temporary!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			const SDL_Rect temp{0, 720, 2276, 1280};
			texture = SDL_CreateTextureFromSurface(renderer, windowSurface);
			SDL_RenderCopyEx(renderer, texture, NULL, &temp, -90, &topLeft, SDL_FLIP_NONE);
		}*/
	//}
	if(UserConfiguration.portraitEnabled){
		SDL_Rect position = {0, -560, windowSurface->w, windowSurface->h};
		SDL_Surface* rot = rotozoomSurface(windowSurface, 90, 1.0, 0);
		SDL_BlitSurface(rot, NULL, windowSurface, &position);
		SDL_FreeSurface(rot);
	}
	SDL_RenderPresent(renderer);
}

void Explore(const char *dir_name){
	DIR *dir;
	entries.clear();
	struct dirent *entry;
	
	dir = opendir(dir_name);
	if(!dir){
		DrawText(fontA, 500, 30, {255, 255, 255, 255}, "Error: Directory not found!");
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
	fontA=TTF_OpenFont("romfs:/font.ttf", 25);
	fontB=TTF_OpenFont("romfs:/icons.ttf", 25);
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		std::cout << "Failed to initialise! Error: " << SDL_GetError( ) << std::endl;
	}
	//window = UserConfiguration.portraitEnabled? SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 1280, 0) : SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//window = SDL_CreateWindow(nullptr, 0, 0, 1280, 1280, 0);
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
			//menu_FileBrowser();
			//if(texture != NULL){
			//	texture = NULL;
			//}
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
	
    TTF_CloseFont(fontA);
	fontA=NULL;
	TTF_CloseFont(fontB);
	fontB=NULL;
	//texture = NULL;
    SDL_FreeSurface( windowSurface );
	windowSurface = NULL;
	//SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
    romfsExit();
	return name;
}