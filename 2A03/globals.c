#include "globals.h"
#include "osc.h"

int sampleSize = 8;
int sampleRate = 48000;
int channels = 1;

int bufferSize = 2048;
u8 *outputBuffer = 0;

int outputPrecision = 6;
useconds_t beatLength = 0;

struct OscData *pOscillators[4];

u8 *pSamples[32];
unsigned int sampleLengths[32];
unsigned int currentFreeSample = 0;

int sampleIndex = -1;
unsigned int sampleOffset = 0;

int done = 0;
int hFile = -1;
useconds_t sleepTime = 0;
pthread_mutex_t mutexDone = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexParse = PTHREAD_MUTEX_INITIALIZER;

