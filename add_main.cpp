/*
*      tetris.c TTY-TETRIS Main file.
*      Copyright © 2008 Martin Duquesnoy <xorg62@gmail.com>
*      All rights reserved.
*
*      Redistribution and use in source and binary forms, with or without
*      modification, are permitted provided that the following conditions are
*      met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following disclaimer
*        in the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the  nor the names of its
*        contributors may be used to endorse or promote products derived from
*        this software without specific prior written permission.
*
*      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <SDL2/SDL.h>
#include "audio.h"
//#include <unistd.h>

void music(const char * filename, int len) {
	/* Init Simple-SDL2-Audio */
	initAudio();
	Audio * music = createAudio(filename, 1, SDL_MIX_MAXVOLUME);
	playMusicFromMemory(music, SDL_MIX_MAXVOLUME);
	SDL_Delay(len);

	/* End Simple-SDL2-Audio */
	endAudio();

	/* Important to free audio after ending Simple-SDL2-Audio because they might be referenced still */
	freeAudio(music);
}

// shape set, unset FYCTION

void sound(const char * filename, int len) {
	initAudio();
	Audio * sound = createAudio(filename, 0, SDL_MIX_MAXVOLUME / 2);
	playSoundFromMemory(sound, SDL_MIX_MAXVOLUME);
	SDL_Delay(len);
	endAudio();
	freeAudio(sound);
}

int main(int argc, char **argv)
{
	/* Initialize only SDL Audio on default device */
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		exit(0);
	}

	sound("test.wav", 2000);
	

	sound("violin.wav", 9000);
	SDL_Quit();

}
