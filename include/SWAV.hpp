#pragma once

#include <vector>
#include <string>

struct SWAV {
	const static int TYPE = 0;
	const static int LOOP_SUPPORT = 1;
	const static int SAMPLE_RATE = 2; //2 bytes
	const static int DURATION = 4; //2 bytes
	const static int LOOP_START = 6; //2 bytes
	const static int LOOP_LENGTH = 8; //4 bytes
	const static int HEADER_SIZE = 12;
	
	const static int ADPCM_HEADER = 12; //4 bytes
	const static int ADPCM_INITIAL_SAMPLE = 0; //2 bytes
	const static int ADPCM_INITIAL_INDEX = 2; //1 byte
	const static int ADPCM_HEADER_SIZE = 4; //4 bytes
	
	const static int PCM8_TYPE = 0;
	const static int PCM16_TYPE = 1; //not intended to support this
	const static int ADPCM_TYPE = 2;
	
	
	int index;
	int type;
	bool loopSupport;
	int sampleRate;
	int duration; //useless
	int loopStart; //index into samples
	int loopLength; //index into samples
	std::vector<short> samples;
	
	std::string info();
};


