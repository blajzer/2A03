/*
Copyright (c) 2009-2010 Brett Lajzer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*
2A03: Super tiny NES-style synth. Now slightly more portable!
*/

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <SDL/SDL.h>

#include "globals.h"
#include "osc.h"
#include "parser.h"

void fillOutputBuffer(void *userdata, Uint8 *stream, int len) {
	int i, j;
	int status;
	
	for(j = 0; j < len; ++j) {
		/* call parser */
		status = parseFile();
		if(status == 0)
			done = 1;
		
		stream[j] = 0;
		
		/* calculate and accumulate the output */
		float outTemp = 0.0;
		for(i = 0; i < 4; ++i) {
			outTemp += osc_getValue( pOscillators[i] );
			osc_advanceOsc( pOscillators[i] );
		}	
		/* TODO: clamp or whatever the value */
		
		stream[j] = osc_convertToNBit(outTemp);
		
		/* add in the raw PCM sample */
		if(sampleIndex != NO_SAMPLE) {
			if(sampleOffset < sampleLengths[sampleIndex]) {
				signed char diff = pSamples[sampleIndex][sampleOffset] - 127;
				stream[j] += diff;
				++sampleOffset;
			} else {
				sampleIndex = NO_SAMPLE;
			}
		}
	}
}

int main(int argc, char **argv) {
	SDL_AudioSpec specDesired;
	SDL_AudioSpec *pSpecObtained = NULL;
	
	if(argc < 2) {
		printf("ERROR: no file specified!\n");
		return 1;
	}
	
	/* init sdl and open the audio */
	SDL_Init(SDL_INIT_AUDIO);
	
	specDesired.freq = sampleRate;
	specDesired.format = AUDIO_U8;
	specDesired.channels = channels;
	specDesired.samples = bufferSize;
	specDesired.callback = fillOutputBuffer;
	SDL_OpenAudio(&specDesired, pSpecObtained);
	
	if(pSpecObtained != NULL) {
		printf("Error: Can't get desired spec.\n");
		return 1;
	}

	/* make the oscillators */
	int i;
	for(i = 0; i < 4; ++i) {
		pOscillators[i] = (struct OscData *)malloc(sizeof(struct OscData));
		pOscillators[i]->wavePos = 0.0f;
		osc_setFreq(pOscillators[i], 1.0f);
		osc_setVolume(pOscillators[i], 0.0f);
		osc_setParam(pOscillators[i], 0.0f);
		osc_setPitchbend(pOscillators[i], 0.0f);
	}
	
	pOscillators[0]->type = SQUARE;
	pOscillators[1]->type = SQUARE;
	pOscillators[2]->type = TRIANGLE;
	pOscillators[3]->type = RANDOM;
	
	/* init sample buffer array */
	for(i = 0; i < 32; ++i) {
		pSamples[i] = NULL;
		sampleLengths[i] = 0;
	}
	
	/* open file */
	hFile = open(argv[1], O_RDONLY);
	
	if(hFile != -1) {
		SDL_PauseAudio(0);
		SDL_LockAudio();
		while(!done) {
			SDL_UnlockAudio();
			SDL_Delay(10);
			SDL_LockAudio();
		}
		SDL_UnlockAudio();
	} else {
		printf("ERROR: can't open file \"%s\"\n", argv[1]);
	}
	
	SDL_CloseAudio();
	SDL_Quit();
	/* clean up */
	for(i = 0; i < 4; ++i)
		free(pOscillators[i]);
	for(i = 0; i < 32; i++)
		if(pSamples[i] != NULL)
			free(pSamples[i]);
	
	return 0;
}

