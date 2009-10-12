/*
	Global declarations
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>
#include <unistd.h>
#include "types.h"

struct OscData; /* forward declaration */

/* sound device settings */
extern int sampleSize;
extern int sampleRate;
extern int channels;

/* output buffer */
extern int bufferSize;
extern u8 *outputBuffer;

/* other settings */
extern int outputPrecision;
extern useconds_t beatLength;

/* oscillators */
extern struct OscData *pOscillators[4];

/* sample buffers */
extern u8 *pSamples[32];
extern unsigned int sampleLengths[32];
extern unsigned int currentFreeSample;

extern int sampleIndex;
extern unsigned int sampleOffset;

/* mutexes */
extern int done;
extern int hFile;
extern useconds_t sleepTime;
extern pthread_mutex_t mutexDone;
extern pthread_mutex_t mutexParse;


#endif

