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

#include "util.h"

#define PRNG_BIT_MASK 0x7FFF
#define PRNG_LONG_BIT_1 0xD
#define PRNG_LONG_BIT_2 0xE
#define PRNG_LONG_MASK_1 (1 << PRNG_LONG_BIT_1)
#define PRNG_LONG_MASK_2 (1 << PRNG_LONG_BIT_2)

#define PRNG_SHORT_BIT_1 0x8
#define PRNG_SHORT_BIT_2 0xE
#define PRNG_SHORT_MASK_1 (1 << PRNG_SHORT_BIT_1)
#define PRNG_SHORT_MASK_2 (1 << PRNG_SHORT_BIT_2)

float clamp_f(float x, float min, float max) {
	return x < min ? min : (x > max ? max : x);
}

u8 clamp_u8(u8 x, u8 min, u8 max) {
	return x < min ? min : (x > max ? max : x);
}

void initPrng(struct LFSR_Prng *prng) {
	prng->reg = 1;
	prng->mode = LFSR_LONG;
}

void updatePrng(struct LFSR_Prng *prng) {
	uint16_t reg = prng->reg; //copy into local
	uint16_t finalBit;
	if(prng->mode == LFSR_LONG) {
		const uint16_t bit1 = (reg & PRNG_LONG_MASK_1) >> PRNG_LONG_BIT_1;
		const uint16_t bit2 = (reg & PRNG_LONG_MASK_2) >> PRNG_LONG_BIT_2;
		finalBit = bit1 ^ bit2;
	} else {
		const uint16_t bit1 = (reg & PRNG_SHORT_MASK_1) >> PRNG_SHORT_BIT_1;
		const uint16_t bit2 = (reg & PRNG_SHORT_MASK_2) >> PRNG_SHORT_BIT_2;
		finalBit = bit1 ^ bit2;
	}
	prng->reg = ((reg << 1) | finalBit) & PRNG_BIT_MASK;
}

