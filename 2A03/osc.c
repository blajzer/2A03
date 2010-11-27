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

#include "osc.h"
#include "globals.h"

#include <stdio.h>
#include <math.h>

void osc_setFreq(struct OscData *pOsc, float freq) {
	if(pOsc && freq > 0.0f) {
		pOsc->freq = freq;
		pOsc->step = (2.0f * M_PI * freq) / (float)sampleRate;
	}
}

void osc_setVolume(struct OscData *pOsc, float vol) {
	if(pOsc) {
		if(vol > 1.0f) {
			vol = 1.0f;
		}else if(vol < 0.0f){
			vol = 0.0f;
		}
		pOsc->volume = vol;
	}
}

void osc_setParam(struct OscData *pOsc, float param) {
	if(pOsc)
		pOsc->param = param;
}

void osc_setPitchbend(struct OscData *pOsc, float pitchbend) {
	if(pOsc)
		pOsc->pitchbend = pitchbend;
}

void osc_advanceOsc(struct OscData *pOsc) {
	pOsc->wavePos += pOsc->step + (pOsc->pitchbend * pOsc->step);
	if(pOsc->wavePos > 2.0f * (float)M_PI)
		pOsc->wavePos -= 2.0f * (float)M_PI;
}

/* returns a value from -1 to 1 */
float osc_getValue(struct OscData *pOsc) {
	float output = 0.0f;
	
	switch(pOsc->type) {
		case SQUARE:
			if(sin(pOsc->wavePos) >= pOsc->param)
				output = 1.0f;
			else
				output = -1.0f;
			break;
		case TRIANGLE:
			output = pOsc->wavePos / (2.0f * M_PI);
			output = output < 0.5f ? output * 4.0f: (1.0f - output) * 4.0f;
			output -= 1.0;
			break;
		case RANDOM:
			break;
	}
	
	return output * pOsc->volume;
}

u8 osc_convertToNBit(float value) {
	u8 output = 0;
	
	/* convert value to range 0 - 1 */
	value = (value / 2.0f) + 0.5f;
	
	/* make 8-bit */
	value *= 255.0f;
	if(value <= 255.0f) {
		if(value >= 0.0f)
			output = (u8)value;
		else
			output = 0;
	}else{
		output = 255;
	}
	
	/* lop off the lower N bits */
	int precision = 8 - outputPrecision;
	output = (output >> precision) << precision;
	
	return output;
}

