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
	
	float step; /* how much to step forward each iteration */
};


/* functions for manipulating the oscillators */
void osc_setFreq(struct OscData *pOsc, float freq);
void osc_setVolume(struct OscData *pOsc, float vol);
void osc_setParam(struct OscData *pOsc, float param);
void osc_advanceOsc(struct OscData *pOsc);

/* function to get the current value of the oscillator */
float osc_getValue(struct OscData *pOsc);

/* function to convert to N-bit (as 8-bit value) */
u8 osc_convertToNBit(float value);

#endif

