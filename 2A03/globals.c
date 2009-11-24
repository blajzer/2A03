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

#include "globals.h"
#include "osc.h"

int sampleSize = 8;
int sampleRate = 48000;
int channels = 1;

int bufferSize = 2048;

int outputPrecision = 6;
unsigned int beatLengthSamples = 0;

struct OscData *pOscillators[4];

u8 *pSamples[32];
unsigned int sampleLengths[32];
unsigned int currentFreeSample = 0;

int sampleIndex = -1;
unsigned int sampleOffset = 0;

int done = 0;
int hFile = -1;
unsigned int samplesToSleep = 0;
