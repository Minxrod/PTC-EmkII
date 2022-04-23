#pragma once

#include <string>
#include <vector>

#include "SWAV.hpp"

struct SWAR {
	const int SAMPLE_COUNT = 0x38;
	const int SAMPLE_OFFSETS_START = 0x3c;
	
	std::vector<char> data;
	std::vector<int> offsets;
	std::vector<SWAV> swav;
	
	void open(std::string);
};
