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
	Global declarations
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <unistd.h>
#include "types.h"
#include "util.h"

#define NO_SAMPLE 255

struct OscData; /* forward declaration */

/* sound device settings */
extern int sampleSize;
extern int sampleRate;
extern int channels;

/* output buffer */
extern int bufferSize;

/* other settings */
extern int outputPrecision;
extern unsigned int beatLengthSamples;

/* oscillators */
extern struct OscData *pOscillators[4];

/* sample buffers */
extern u8 *pSamples[32];
extern unsigned int sampleLengths[32];
extern unsigned int currentFreeSample;

extern int sampleIndex;
extern unsigned int sampleOffset;


/* random channel data */
extern struct LFSR_Prng prng;

/* parsing vars */
extern int done;
extern int hFile;
extern unsigned int samplesToSleep;
#endif

