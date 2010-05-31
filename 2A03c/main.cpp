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

/* 
	2A03c a compiler for the 2A03 synth assembly language
*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>


using namespace std;

//constants
const char INSTR_FORM_MASK    = 0b10000000;
const char INSTR_OPCODE_MASK  = 0b01111100;
const char INSTR_CHANNEL_MASK = 0b00000011;

struct MnemonicInfo {
	MnemonicInfo(const char *m, int o, int f): mnemonic(m), opcode(o), form(f){};
	string mnemonic;
	int opcode;
	int form;
};

//table for mnemonics to instruction form
const int INSTR_TABLE_SIZE = 9;
MnemonicInfo mnemonicTable[INSTR_TABLE_SIZE] = {
	MnemonicInfo( "nop" ,0 ,0 ),
	MnemonicInfo( "prc" ,1 ,1 ),
	MnemonicInfo( "bpm" ,2 ,1 ),
	MnemonicInfo( "rst" ,3 ,1 ),
	MnemonicInfo( "frq" ,4 ,2 ),
	MnemonicInfo( "vol" ,5 ,2 ),
	MnemonicInfo( "par" ,6 ,2 ),
	MnemonicInfo( "pcl" ,8 ,3 ),
	MnemonicInfo( "pcp" ,9 ,1 ),
};


//file streams
fstream fileIn, fileOut;

//error-tracking stuff
int lineNum = 0;
string currentLine = "";

//current instruction stuff
bool isLongForm = false;
char instr[4] = {0,0,0,0};

//helper-functions
void setInstrForm(bool longForm) {
	isLongForm = longForm;
	if(longForm){
		instr[0] |= INSTR_FORM_MASK;
	} else {
		instr[0] &= ~INSTR_FORM_MASK;
	}
}

void setInstrOpcode(int opcode) {
	instr[0] &= ~INSTR_OPCODE_MASK;
	instr[0] |= (opcode << 2) & INSTR_OPCODE_MASK;
}

void setInstrChannel(int channel) {
	instr[0] &= ~INSTR_CHANNEL_MASK;
	instr[0] |= channel & INSTR_CHANNEL_MASK;
}

void setFormChannelOpcode(bool longForm, int channel, int opcode) {
	setInstrForm(longForm);
	setInstrOpcode(opcode);
	setInstrChannel(channel);
}

void setInstrData(int data) {
	if(isLongForm) {
		instr[1] = (data >> 16) & 0xFF;
		instr[2] = (data >> 8) & 0xFF;
		instr[3] = data & 0xFF;	
	} else {
		instr[1] = data & 0xFF;
	}
}

//converts a mnemonic to a instruction form
int decodeMnemonic(const string &m, int &op, int &type) {
	for(int i = 0; i < INSTR_TABLE_SIZE; ++i) {
		if(mnemonicTable[i].mnemonic == m) {
			op = mnemonicTable[i].opcode;
			type = mnemonicTable[i].form;
			return 0;
		}
	}

	return -1;
}

//writes the instruction out to the output file
void writeInstruction() {
	fileOut << instr[0] << instr[1];

	if(isLongForm) {
		fileOut << instr[2] << instr[3];
	}
}

// processes a given line, looking for an instruction
// errors if none found, else writes intruction into instr[]
void processLine(stringstream &line) {
	string mnemonic;
	int instrType = 0;
	int instrOpcode = 0;
	bool shouldWrite = true;

	line >> mnemonic; //read out the mnemonic
	if( mnemonic != "" && mnemonic[0] != '#' && decodeMnemonic( mnemonic, instrOpcode, instrType ) == -1) {
		cerr << "ERROR:"<< lineNum <<": Invalid instruction - " << mnemonic << endl;
		exit(1);
	}

	if(mnemonic == "" || mnemonic[0] == '#') {
		instrType = -1;
		shouldWrite = false;
	}

	//read in the arguments based on the form and then compose the instruction
	switch(instrType) {
		case -1: //a comment
			break;
		case 0: //nop
			instr[0] = 0;
			instr[1] = 0;
			isLongForm = false;
			break;
		case 1: // <mnemonic> <value>
		{
			int value;
			line >> value;
			if(line.fail() || value < 0 || value >= (1 << 24)) {
				cerr << "ERROR:"<< lineNum <<": Invalid argument - " << value << endl;
				exit(1);
			}
			
			if(value > 255) {
				setFormChannelOpcode(true, 0, instrOpcode);
			} else {
				setFormChannelOpcode(false, 0, instrOpcode);
			}

			setInstrData(value);

			break;
		}
		case 2:
		{
			int channel, value;
			line >> channel;
			if(line.fail() || channel < 0 || channel > 3 ) {
				cerr << "ERROR:"<< lineNum <<": Invalid channel - " << value << endl;
				exit(1);
			}

			line >> value;
			if(line.fail() || value < 0 || value >= (1 << 24)) {
				cerr << "ERROR:"<< lineNum <<": Invalid argument - " << value << endl;
				exit(1);
			}

			if(value > 255) {
				setFormChannelOpcode(true, channel, instrOpcode);
			} else {
				setFormChannelOpcode(false, channel, instrOpcode);
			}

			setInstrData(value);
			break;
		}
		case 3:
		{
			//try to get path
			char pcmPath[1024];
			line.getline(&pcmPath[0], 1023);
			pcmPath[line.gcount()] = 0;
			
			if(line.fail()) {
				cerr << "ERROR:"<< lineNum <<": Invalid pcm load instruction." << endl;
				exit(1);
			}
			
			//clean up path (remove quotes and surrounding whitespace)
			string cleanPath(pcmPath);
			size_t firstQuote = cleanPath.find_first_of('\"');
			size_t lastQuote = cleanPath.find_last_of('\"');
			
			if(firstQuote == string::npos || lastQuote == string::npos || firstQuote == lastQuote || lastQuote == firstQuote + 1) {
				cerr << "ERROR:"<< lineNum <<": Invalid pcm load instruction." << endl;
				exit(1);
			}
			
			cleanPath = cleanPath.substr(firstQuote + 1, lastQuote - 2);
			
			//open file
			ifstream pcmFile;
			pcmFile.open(cleanPath.c_str());
			if(pcmFile.fail()) {
				cerr << "ERROR:"<< lineNum <<": Can't open file: \"" << cleanPath << "\"" << endl;
				exit(1);
			}
			
			//read out the data
			//TODO: read in chunks and use less memory
			stringstream pcmData;
			char c;
			while(pcmFile.good()) {
				c = pcmFile.get();
				if(pcmFile.good())
					pcmData << c;
			}
			
			size_t dataLen = pcmData.str().length();
			if(dataLen > 255)
				setFormChannelOpcode(true, 0, instrOpcode);
			else
				setFormChannelOpcode(false, 0, instrOpcode);
				
			setInstrData(dataLen);
			writeInstruction();
			
			//write out the data
			fileOut.write(pcmData.str().c_str(), dataLen);
			
			shouldWrite = false;
			break;
		}
		default:
		{
			
		}
	}

	//output the instruction to the output file
	if(shouldWrite) {
		writeInstruction();
	}

}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		cout << "USAGE: " << argv[0] << " <input file> <output file>" << endl;
		return 1;
	}

	//open up the files
	fileIn.open(argv[1], ios_base::in);
	fileOut.open(argv[2], ios_base::out | ios_base::binary | ios_base::trunc );

	if( fileIn.fail() ) {
		cerr << "ERROR: Cannot open file for reading: \"" << argv[2] << "\"" << endl;
		return 1;
	} else {
		cout << "Opened source file: \"" << argv[1] << "\"" << endl;
	}
	
	if( fileOut.fail() ) {
		cerr << "ERROR: Cannot open file for writing: \"" << argv[2] << "\"" << endl;
		return 1;
	} else {
		cout << "Opened output file: \"" << argv[2] << "\"" << endl;
	}

	char readBuffer[256];
	stringstream bufferStream;
	while(!fileIn.eof()) {
		++lineNum;

		fileIn.getline(readBuffer, 256);
		bufferStream << readBuffer;

		processLine( bufferStream );

		bufferStream.str("");
		bufferStream.clear();
	}

	fileIn.close();
	fileOut.close();

	return 0;
}

