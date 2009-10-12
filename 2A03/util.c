#include "util.h"

float clamp_f(float x, float min, float max) {
	return x < min ? min : (x > max ? max : x);
}

u8 clamp_u8(u8 x, u8 min, u8 max) {
	return x < min ? min : (x > max ? max : x);
}

