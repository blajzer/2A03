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
Simple, configurable square wave/triangle oscillator, also has random mode
*/

#ifndef OSC_C
#define OSC_C

#include "types.h"

enum OscType {
	SQUARE = 0,
	TRIANGLE = 1,
	RANDOM = 2
};

struct OscData {
	enum OscType type;
	float wavePos; /* the current position within the wave */
	float freq; /* the frequency */
	float param; /* extra parameter */
	float volume; /* volume */
	float pitchbend; /* pitchbend amount, modifies step */
	
	float step; /* how much to step forward each iteration */
};


/* functions for manipulating the oscillators */
void osc_setFreq(struct OscData *pOsc, float freq);
void osc_setVolume(struct OscData *pOsc, float vol);
void osc_setParam(struct OscData *pOsc, float param);
void osc_setPitchbend(struct OscData *pOsc, float pitchbend);
void osc_advanceOsc(struct OscData *pOsc);

/* function to get the current value of the oscillator */
float osc_getValue(struct OscData *pOsc);

/* function to convert to N-bit (as 8-bit value) */
u8 osc_convertToNBit(float value);

#endif

