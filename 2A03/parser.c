#include "parser.h"
#include "globals.h"
#include "util.h"
#include "osc.h"

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static const u8 LONG_FORM    = 0b10000000;
static const u8 OPCODE_MASK  = 0b01111100;
static const u8 CHANNEL_MASK = 0b00000011;

static const float MINUTE_LENGTH = 60.0f * 1000000.0f;

static const int OPCODE_SHIFT = 2;

void *parseFile(void *p) {
	u8 instr, d1, d2, d3; /* what we're reading into */
	u8 opcode, channel;  /* decoded instruction */
	unsigned int data = 0;  /* decoded data */
	
	int doSleep = 0;
	
	/* start reading the file */
	ssize_t readBytes;
	readBytes = read(hFile, &instr, 1);
	while(readBytes > 0) {
		opcode = (OPCODE_MASK & instr) >> OPCODE_SHIFT; /* get opcode */
		channel = CHANNEL_MASK & instr;
		
		/* get data */
		if(instr & LONG_FORM) { /* long form -- ugly */
			readBytes = read(hFile, &d1, 1);
			if(readBytes > 0) {
				readBytes = read(hFile, &d2, 1);
				if(readBytes > 0) {
					readBytes = read(hFile, &d3, 1);
					if(readBytes > 0) {
						data = bytesToData(d1, d2, d3);
					}
				}
			}
		}else{ /* short form */
			readBytes = read(hFile, &d1, 1);
			if(readBytes > 0)
				data = (unsigned int)d1;
		}

		/* execute the instruction */
		pthread_mutex_lock(&mutexParse);
		doSleep = executeInstruction(opcode, channel, data);
		pthread_mutex_unlock(&mutexParse);
		
		if(doSleep)
			usleep(sleepTime);
		
		/* continue reading */
		readBytes = read(hFile, &instr, 1);
	}
	pthread_mutex_lock(&mutexDone);
	done = 1;
	pthread_mutex_unlock(&mutexDone);
	
	return 0;
}

int executeInstruction(u8 opcode, u8 channel, unsigned int data) {
	float fData = (float)data;
	
	switch(opcode) {
		case 0: /* nop */
			break;
		case 1: /* precision */
			outputPrecision = clamp_u8(data, 1, 8);
			break;
		case 2: /* tempo */
			if(fData < 60.0f)
				fData = 60.0f;
				
			beatLength = (useconds_t)(MINUTE_LENGTH / fData);
			break;
		case 3: /* rest */
			sleepTime = beatLength * (useconds_t)data;
			return 1;
			break;
		case 4: /* freq */
			if(fData < 1.0f)
				fData = 1.0f;

			osc_setFreq(pOscillators[channel], fData / 1000.0f);
			break;
		case 5: /* volume */
			osc_setVolume(pOscillators[channel], fData / 255.0f );
			break;
		case 6: /* param */
			osc_setParam(pOscillators[channel], fData / 255.0f );
			break;
		case 7: /* pitchbend - TODO */
			break;
		case 8: /* PCM load */
			if(currentFreeSample == 32)
				return 0;

			/* allocate the buffer */
			pSamples[currentFreeSample] = (u8 *)malloc(data * sizeof(u8));
			sampleLengths[currentFreeSample] = data;
			
			/* read in the sample */
			read(hFile, pSamples[currentFreeSample], data);
			++currentFreeSample;
			break;
		case 9: /* PCM play */
			if(data >= 0 && data < 32 && pSamples[data] != NULL) {
				sampleIndex = data;
				sampleOffset = 0;
			}
			break;
	};
	
	return 0;
}

unsigned int bytesToData(u8 b1, u8 b2, u8 b3) {
	return (unsigned int)b3 | ( ((unsigned int)b2) << 8 ) | ( ((unsigned int)b1) << 16 );
}

