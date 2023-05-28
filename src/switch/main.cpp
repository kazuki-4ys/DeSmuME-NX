/* main.c - this file is part of DeSmuME
*
* Copyright (C) 2006,2007 DeSmuME Team
* Copyright (C) 2007 Pascal Giard (evilynux)
* Copyright (C) 2009 Yoshihiro (DsonPSP)
* This file is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; see the file COPYING.  If not, write to
* the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*/
#include <stdio.h>
#include <switch.h>

#include <malloc.h>

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)

// GLM headers
#define GLM_FORCE_PURE
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../MMU.h"
#include "../NDSSystem.h"
#include "../debug.h"
#include "../render3D.h"
#include "../rasterize.h"
#include "../saves.h"
#include "../mic.h"
#include "../SPU.h"

#include "input.h"
#include "sound.h"
#include "config.h"

#include "../opengl/mesh.h"

volatile bool execute = FALSE;

PadState pad;

EGLDisplay s_display;
EGLContext s_context;
EGLSurface s_surface;

bool initEgl(NWindow* win)
{
    // Connect to the EGL default display
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!s_display)
    {
        goto _fail0;
    }

    // Initialize the EGL display connection
    eglInitialize(s_display, nullptr, nullptr);

    // Select OpenGL (Core) as the desired graphics API
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
    {
        goto _fail1;
    }

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint framebufferAttributeList[] =
    {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE,     8,
        EGL_GREEN_SIZE,   8,
        EGL_BLUE_SIZE,    8,
        EGL_ALPHA_SIZE,   8,
        EGL_DEPTH_SIZE,   24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    eglChooseConfig(s_display, framebufferAttributeList, &config, 1, &numConfigs);
    if (numConfigs == 0)
    {
        goto _fail1;
    }

    // Create an EGL window surface
    s_surface = eglCreateWindowSurface(s_display, config, win, nullptr);
    if (!s_surface)
    {
        goto _fail1;
    }

    // Create an EGL rendering context
    static const EGLint contextAttributeList[] =
    {
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
        EGL_CONTEXT_MINOR_VERSION_KHR, 3,
        EGL_NONE
    };
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, contextAttributeList);
    if (!s_context)
    {
        goto _fail2;
    }

    // Connect the context to the surface
    eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    return true;

_fail2:
    eglDestroySurface(s_display, s_surface);
    s_surface = nullptr;
_fail1:
    eglTerminate(s_display);
    s_display = nullptr;
_fail0:
    return false;
}

void deinitEgl(void)
{
    if (s_display)
    {
        eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (s_context)
        {
            eglDestroyContext(s_display, s_context);
            s_context = nullptr;
        }
        if (s_surface)
        {
            eglDestroySurface(s_display, s_surface);
            s_surface = nullptr;
        }
        eglTerminate(s_display);
        s_display = nullptr;
    }
}


static inline uint8_t convert5To8(uint8_t x)
{
	return ((x << 3) | (x >> 2));
}

static inline uint32_t ABGR1555toRGBA8(uint16_t color)
{
	uint32_t pixel = 0x00;
	uint8_t  *dst = (uint8_t*)&pixel; 

	dst[0] = convert5To8((color >> 0)   & 0x1F); //R
	dst[1] = convert5To8((color >> 5)   & 0x1F); //G
	dst[2] = convert5To8((color >> 10)  & 0x1F); //B
	dst[3] = 0xFF;//CONVERT_5_TO_8((color >> 11) & 0x1F); //A

    return pixel;
}

GPU3DInterface *core3DList[] = {
	&gpu3DNull,
	&gpu3DRasterize,
	NULL
};

SoundInterface_struct *SNDCoreList[] = {
  &SNDDummy,
  &SNDDummy,
  &SNDSwitch,
  NULL
};

const char * save_type_names[] = {
	"Autodetect",
	"EEPROM 4kbit",
	"EEPROM 64kbit",
	"EEPROM 512kbit",
	"FRAM 256kbit",
	"FLASH 2mbit",
	"FLASH 4mbit",
	NULL
};

int cycles;

static unsigned short keypad;

unsigned int calcFrameBufOffset(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
	return ((height - 1) - y) * width + x;
}

static void desmume_cycle()
{
    process_ctrls_events(&keypad);

    //touchPosition touch;
	//hidTouchRead(&touch, 0);
		
	/*if(UserConfiguration.portraitEnabled){
		if(touch.px > 538.9473684210526 && touch.px < 1077.894736842105 && touch.py > 0 && touch.py < 720){
			NDS_setTouchPos((-touch.py + 720) / 2.807017543859649, (touch.px - 540) / 2.8125);
		} else {
            NDS_releaseTouch();
        }
			//NDS_setTouchPos((-touch.py + 720) / 2.807017543859649, (touch.px - 540) / 2.8125);//Working solution!		
	}*/
		
	/*else if(!UserConfiguration.portraitEnabled){
		if(touch.px > 401 && touch.px < 882 && touch.py > 360 && touch.py < 720){
			NDS_setTouchPos((touch.px - 401) / 1.875,(touch.py - 360) / 1.875);
		} else {
            NDS_releaseTouch();
        }
	}*/

	update_keypad(keypad);
	
    NDS_exec<false>();

    if(UserConfiguration.soundEnabled)
    	SPU_Emulate_user();
}

int main(int argc, char **argv)
{
	romfsInit();
	unsigned int screenW, screenH;
	//char *rom_path = menu_FileBrowser();
	const char *rom_path = "sdmc:/switch/desmume/rom.nds";
	//FILE *file;
	//file = fopen("sdmc:/Switch/desmume/log2.txt", "w");
	//fwrite(rom_path, 3, 100, file);
	//fclose(file);
	
	//gfxInitDefault();
	
	//consoleInit(NULL);
	// Retrieve the default window
    NWindow* win = nwindowGetDefault();

    // Create a linear double-buffered framebuffer
    Framebuffer fb;
	if(UserConfiguration.portraitEnabled){
		screenW = 456;
		screenH = 256;
	}else{
		screenW = 684;
		screenH = 384;
	}
	unsigned int framebufferSize = screenW * screenH * 4;
	unsigned char *glTexRaw = (unsigned char*)calloc(framebufferSize, sizeof(unsigned char));
	initEgl(win);
    gladLoadGL();

	TwoDDynamicTexQuad* desmumeRenderer = new TwoDDynamicTexQuad();
    desmumeRenderer->posZ = 0.1f;
	desmumeRenderer->run1Fr(0.0f);

	//UserConfiguration.portraitEnabled ? gfxConfigureResolution(456, 256) : gfxConfigureResolution(684, 384);

	/* the firmware settings */
	struct NDS_fw_config_data fw_config;
	
	int xScale = 256;

	/* default the firmware settings, they may get changed later */
	NDS_FillDefaultFirmwareConfigData(&fw_config);

  	NDS_Init();

  	//GPU->Change3DRendererByID(RENDERID_OPENGL_AUTO);
	GPU->Change3DRendererByID(RENDERID_SOFTRASTERIZER);
  	SPU_ChangeSoundCore(SNDCORE_SWITCH, 735 * 4);

	CommonSettings.loadToMemory = true;

	if (NDS_LoadROM( rom_path, NULL, NULL) < 0) {
		printf("Error loading %s\n", rom_path);
	}

	execute = TRUE;
	u32 width, height;
	uint32_t keysDown;
	unsigned int stride;

	padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);

	while(appletMainLoop()) 
	{
		padUpdate(&pad);
		for (int i = 0; i < UserConfiguration.frameSkip; i++)
		{
			NDS_SkipNextFrame();
			desmume_cycle();
		}

		desmume_cycle();

		uint16_t * src = (uint16_t *)GPU->GetDisplayInfo().masterNativeBuffer;
		//uint32_t *framebuffer = (uint32_t*)gfxGetFramebuffer(&width, &height);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(int x = 0; x < xScale; x++){
    		for(int y = 0; y < (192 * 2); y++){
    			//uint32_t offset = UserConfiguration.portraitEnabled ? gfxGetFramebufferDisplayOffset(y, -x + (height / 2) + (xScale / 2) - 2) : gfxGetFramebufferDisplayOffset(214 + x, y);
        		//framebuffer[offset] = ABGR1555toRGBA8(src[((y * xScale) + x)]);
				unsigned int color;
				if(UserConfiguration.portraitEnabled){
					color = ABGR1555toRGBA8(src[((y * xScale) + x)]);
					memcpy(glTexRaw + (calcFrameBufOffset(y, -x + (height / 2) + (xScale / 2) - 2, screenW, screenH) * 4), &color, 4);
				}else{
					color = ABGR1555toRGBA8(src[((y * xScale) + x)]);
					memcpy(glTexRaw + (calcFrameBufOffset(214 + x, y, screenW, screenH) * 4), &color, 4);
				}
    		}
		}
		desmumeRenderer->setTex(glTexRaw, screenW, screenH);
		desmumeRenderer->render();

		eglSwapBuffers(s_display, s_surface);//フレームバッファのスワップ
		//gfxFlushBuffers();
		//gfxSwapBuffers();
		
		//keysDown = hidKeysDown(CONTROLLER_P1_AUTO);
		keysDown = padGetButtonsDown(&pad);
		if(keysDown &  HidNpadButton_Minus){
			if(keysDown & HidNpadButton_Plus){
				NDS_FreeROM();
				//KillDisplay();
				NDS_DeInit();
				//NDS_Quit;
				break;
			}
		}else if(keysDown & HidNpadButton_Plus){
			if(keysDown &  HidNpadButton_Minus){
				NDS_FreeROM();
				//KillDisplay();
				NDS_DeInit();
				//NDS_Quit;
				break;
			}
		}
    }
	delete desmumeRenderer;
	deinitEgl();
	//gfxExit();
	free(glTexRaw);
	romfsExit();
	return 0;
}