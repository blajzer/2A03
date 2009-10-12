#ifndef PARSER_H
#define PARSER_H

#include "types.h"

void *parseFile(void *);

int executeInstruction(u8 opcode, u8 channel, unsigned int data);

unsigned int bytesToData(u8 b1, u8 b2, u8 b3);

#endif

