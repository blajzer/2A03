/*
Copyright (c) 2009 Brett Lajzer

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
2A03: Super tiny NES-style synth. Barely portable, even on Linux.

*/

#include <stdlib.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "linux/soundcard.h"

#include "globals.h"
#include "osc.h"
#include "parser.h"

void fillOutputBuffer() {
	int i, j;
	
	for(j = 0; j < bufferSize; ++j) {
		/* calculate and accumulate the output */
		float outTemp = 0.0;
		pthread_mutex_lock(&mutexParse);
		for(i = 0; i < 4; ++i) {
			outTemp += osc_getValue( pOscillators[i] );
			osc_advanceOsc( pOscillators[i] );
		}
		pthread_mutex_unlock(&mutexParse);
		/* TODO: clamp or whatever the value */
		
		
		outputBuffer[j] = osc_convertToNBit(outTemp);
	}
}

int main(int argc, char **argv) {
	int hDSP;
	int arg;
	int status;
	pthread_t thread;
	
	if(argc < 2) {
		printf("ERROR: no file specified!\n");
		return 1;
	}
	
	/* open output device */
	hDSP = open("/dev/dsp", O_WRONLY);
	if(hDSP < 0) {
		printf("ERROR:%d:Can't open /dev/dsp!\n", hDSP);
		return 1;
	}
	
	/* set parameters */
	arg = sampleSize;
	status = ioctl(hDSP, SOUND_PCM_WRITE_BITS, &arg);
	if(status == -1 || arg != sampleSize) {
		printf("ERROR: Can't set sample size to 8-bits.\n");
		return 1;
	}
	
	arg = channels;
	status = ioctl(hDSP, SOUND_PCM_WRITE_CHANNELS, &arg);
	if(status == -1 || arg != channels) {
		printf("ERROR: Can't set mono output.\n");
		return 1;
	}
	
	arg = sampleRate;
	status = ioctl(hDSP, SOUND_PCM_WRITE_RATE, &arg);
	if(status == -1 || arg != sampleRate) {
		printf("Can't set sample rate to %d, setting to %d instead.\n", sampleRate, arg);
		sampleRate = arg;
	}
	
	/* create mutexes */
	pthread_mutex_init(&mutexDone, NULL);
	pthread_mutex_init(&mutexParse, NULL);
	
	/* init the state and output buffer */
	outputBuffer = malloc(bufferSize * sizeof(u8)); 

	/* make the oscillators */
	int i;
	for(i = 0; i < 4; ++i)
		pOscillators[i] = (struct OscData *)malloc(sizeof(struct OscData));
	
	pOscillators[0]->type = SQUARE;
	pOscillators[0]->wavePos = 0.0f;
	osc_setFreq(pOscillators[0], 1.0f);
	osc_setVolume(pOscillators[0], 0.0f);
	osc_setParam(pOscillators[0], 0.0f);
	
	pOscillators[1]->type = SQUARE;
	pOscillators[1]->wavePos = 0.0f;
	osc_setFreq(pOscillators[1], 1.0f);
	osc_setVolume(pOscillators[1], 0.0f);
	osc_setParam(pOscillators[1], 0.0f);
	
	pOscillators[2]->type = TRIANGLE;
	pOscillators[2]->wavePos = 0.0f;
	osc_setFreq(pOscillators[2], 1.0f);
	osc_setVolume(pOscillators[2], 0.0f);
	osc_setParam(pOscillators[2], 0.0f);
	
	pOscillators[3]->type = RANDOM;
	pOscillators[3]->wavePos = 0.0f;
	osc_setFreq(pOscillators[3], 1.0f);
	osc_setVolume(pOscillators[3], 0.0f);
	osc_setParam(pOscillators[3], 0.0f);
	
	/* init sample buffer array */
	for(i = 0; i < 32; ++i) {
		pSamples[i] = NULL;
		sampleLengths[i] = 0;
	}
	
	/* open file */
	hFile = open(argv[1], O_RDONLY);
	
	if(hFile != -1) {
		/* create parsing thread */
		pthread_create(&thread, NULL, parseFile, NULL);
	
		pthread_mutex_lock(&mutexDone);
		while(!done) {
			pthread_mutex_unlock(&mutexDone);
		
			fillOutputBuffer();
			write(hDSP, outputBuffer, bufferSize);
		
			pthread_mutex_lock(&mutexDone);
		}
		pthread_mutex_unlock(&mutexDone);
	} else {
		printf("ERROR: can't open file \"%s\"\n", argv[1]);
	}
	
	/* clean up */
	for(i = 0; i < 4; ++i)
		free(pOscillators[i]);
	close(hDSP);
	free(outputBuffer);
	pthread_mutex_destroy(&mutexDone);
	pthread_mutex_destroy(&mutexParse);
	
	return 0;
}

