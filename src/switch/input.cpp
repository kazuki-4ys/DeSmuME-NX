/* input.c - this file is part of DeSmuME
 *
 * Copyright (C) 2007 Pascal Giard
 * Copyright (C) 2016 Felipe Izzo
 *
 * Author: Pascal Giard <evilynux@gmail.com>
 *
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

#include "input.h"
#include "NDSSystem.h"

#include <switch.h>

#define BIT(n) (1U<<(n))

extern PadState pad;

 /* Update NDS keypad */
void update_keypad(u16 keys)
{
    // Set raw inputs
  {
    buttonstruct<bool> input = {};
    input.G = (keys>>12)&1;
    input.E = (keys>>8)&1;
    input.W = (keys>>9)&1;
    input.X = (keys>>10)&1;
    input.Y = (keys>>11)&1;
    input.A = (keys>>0)&1;
    input.B = (keys>>1)&1;
    input.S = (keys>>3)&1;
    input.T = (keys>>2)&1;
    input.U = (keys>>6)&1;
    input.D = (keys>>7)&1;
    input.L = (keys>>5)&1;
    input.R = (keys>>4)&1;
    input.F = (keys>>14)&1;
    //RunAntipodalRestriction(input);
    NDS_setPad(
      input.R, input.L, input.D, input.U,
      input.T, input.S, input.B, input.A,
      input.Y, input.X, input.W, input.E,
      input.G, input.F);
  }
  
  // Set real input
  NDS_beginProcessingInput();
  {
    UserButtons& input = NDS_getProcessingUserInput().buttons;
    //ApplyAntipodalRestriction(input);
  }
  NDS_endProcessingInput();
}

//HidControllerKeys keymap[] = {KEY_A, KEY_B, KEY_MINUS, KEY_PLUS, KEY_DRIGHT, KEY_DLEFT, KEY_DUP, KEY_DOWN, KEY_R, KEY_L, KEY_X, KEY_Y };
unsigned int keymap[] = {HidNpadButton_A, HidNpadButton_B, HidNpadButton_Minus, HidNpadButton_Plus, HidNpadButton_Right, HidNpadButton_Left, HidNpadButton_Up, HidNpadButton_Down, HidNpadButton_R, HidNpadButton_L, HidNpadButton_X, HidNpadButton_Y };


/* Manage input events */
int process_ctrls_events( u16 *keypad )
{
	//hidScanInput();

	//uint32_t kDown = hidKeysDown(CONTROLLER_P1_AUTO);
  uint32_t kDown = padGetButtonsDown(&pad);
	//uint32_t kUp   = hidKeysUp(CONTROLLER_P1_AUTO);
  uint32_t kUp   = padGetButtonsUp(&pad);

	for(int i=0; i < 12; i++)
	{
		if(kDown & keymap[i])
			ADD_KEY( *keypad, BIT(i));
		if(kUp & keymap[i])
			RM_KEY( *keypad,  BIT(i));
	}

	return 0;
}