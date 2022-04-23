#pragma once

#include <string>
#include <vector>

//https://gota7.github.io/NitroStudio2/specs/bank.html
//also GBATek 

struct NoteDefinition {
	const static int SWAV_NUMBER = 0; //size 2
	const static int SWAR_NUMBER = 2; //size 2
	const static int NOTE = 4; //size 1
	const static int ATTACK = 5; //size 1
	const static int DECAY = 6; //size 1
	const static int SUSTAIN = 7; //size 1
	const static int RELEASE = 8; //size 1
	const static int PAN = 9; //size 1
	
	int swav_no;
	int swar_no;
	int note;
	int attack;
	int decay;
	int sustain;
	int release;
	int pan;
	
	void read(char* data);
	
	std::string info();
};

struct Instrument {
	const static int F_RECORD_EMPTY = 0;
	const static int F_RECORD_PCM = 1;
	const static int F_RECORD_PSG = 2;
	const static int F_RECORD_NOISE = 3;
	const static int F_RECORD_RANGE = 16;
	const static int F_RECORD_REGIONAL = 17;
	
	int id;
	int f_record;
	std::vector<int> note_numbers;
	std::vector<NoteDefinition> notes;
	
	void read(char* data);
	std::string info();
	
	NoteDefinition& get_note_def(int);
};

struct SBNK {
	const int INSTRUMENT_COUNT = 0x38;
	const int INSTRUMENT_RECORDS_START = 0x3c;
	const int INSTRUMENT_F_RECORD = 0; //size 1
	const int INSTRUMENT_OFFSET = 1; //size 2
	const int INSTRUMENT_RECORD_SIZE = 4; //1 unused byte (always 00?)
	
	std::vector<char> data;
	std::vector<int> offsets;
	std::vector<Instrument> instruments;
	
	void open(std::string name);
};
