/*
   Copyright (C) 2016 Felipe Izzo <MasterFeizz>
   Copyright (C) 2005-2015 DeSmuME Team
   Copyright 2005-2006 Theo Berkau

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "types.h"
#include "SPU.h"
#include "sound.h"
#include "debug.h"

#include <switch.h>

#define ALIGN_TO(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define SWITCH_OUTPUT_BUFFER_SAMPLES 4096
#define SWITCH_OUTPUT_BUFFER_SIZE    4096 * 4

int  SNDSwitchInit(int buffersize);
void SNDSwitchDeInit();
void SNDSwitchUpdateAudio(s16 *buffer, u32 num_samples);
u32  SNDSwitchGetAudioSpace();
void SNDSwitchMuteAudio();
void SNDSwitchUnMuteAudio();
void SNDSwitchSetVolume(int volume);

SoundInterface_struct SNDSwitch = {
	SNDCORE_SWITCH,
	"Switch Sound Interface",
	SNDSwitchInit,
	SNDSwitchDeInit,
	SNDSwitchUpdateAudio,
	SNDSwitchGetAudioSpace,
	SNDSwitchMuteAudio,
	SNDSwitchUnMuteAudio,
	SNDSwitchSetVolume
};

static bool soundInitialized = 0;
static bool soundTerminate   = 0;

static Thread  soundThread;
static CondVar soundReady;
static Mutex   soundMutex;

static AudioOutBuffer soundOutBuffer[2], *releasedBuffer;
static uint32_t releasedCount;

static void soundTask(void *)
{
	while(!soundTerminate)
	{
		condvarWait(&soundReady, &soundMutex);
		//condvarWait(&soundReady);
		audoutWaitPlayFinish(&releasedBuffer, &releasedCount, 0xFFFFFFFFFFFFFFFFLLU);
		mutexUnlock(&soundMutex);
	}
}

//////////////////////////////////////////////////////////////////////////////

int SNDSwitchInit(int buffersize)
{
	if(soundInitialized)
		return 0;
	condvarInit(&soundReady);
	//condvarInit(&soundReady, &soundMutex);

	audoutInitialize();
	audoutStartAudioOut();

	for (int i = 0; i < 2; i++)
	{
		soundOutBuffer[i].buffer = (uint8_t*)memalign(0x1000, ALIGN_TO(SWITCH_OUTPUT_BUFFER_SIZE, 0x1000));
		soundOutBuffer[i].next = NULL;
		soundOutBuffer[i].buffer_size = SWITCH_OUTPUT_BUFFER_SIZE;
		soundOutBuffer[i].data_size   = SWITCH_OUTPUT_BUFFER_SIZE;
		soundOutBuffer[i].data_offset = 0;

		audoutAppendAudioOutBuffer(&soundOutBuffer[i]);
	}

	audoutWaitPlayFinish(&releasedBuffer, &releasedCount, 0xFFFFFFFFFFFFFFFFLLU);

	threadCreate(&soundThread, &soundTask, NULL, nullptr, 0x1000, 0x20, 3);

	threadStart(&soundThread);

	soundInitialized = 1;

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void SNDSwitchDeInit()
{

}

//////////////////////////////////////////////////////////////////////////////

void SNDSwitchUpdateAudio(s16 *buffer, u32 num_samples)
{
	mutexLock(&soundMutex);

	memcpy(releasedBuffer->buffer, buffer, num_samples * sizeof(s16));
	releasedBuffer->buffer_size = num_samples * sizeof(s16);
	releasedBuffer->data_size   = num_samples * sizeof(s16);
	audoutAppendAudioOutBuffer(releasedBuffer);
	condvarWakeOne(&soundReady);

	mutexUnlock(&soundMutex);
}

//////////////////////////////////////////////////////////////////////////////

u32 SNDSwitchGetAudioSpace()
{
	return SWITCH_OUTPUT_BUFFER_SIZE;
}

//////////////////////////////////////////////////////////////////////////////

void SNDSwitchMuteAudio()
{

}

//////////////////////////////////////////////////////////////////////////////

void SNDSwitchUnMuteAudio()
{

}

//////////////////////////////////////////////////////////////////////////////

void SNDSwitchSetVolume(int volume)
{
}

//////////////////////////////////////////////////////////////////////////////