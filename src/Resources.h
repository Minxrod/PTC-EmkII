#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <map>

#include "Vars.h"

struct PRG {
	std::vector<unsigned char> data;
};

struct CHR{
	std::array<unsigned char, 256*8*8/2> data;
};

struct MEM{
	std::array<unsigned char, 256*2> data;
	
	std::string get_mem();
};

struct SCR{
	std::array<uint16_t, 64*64> data;
};

struct COL{
	std::array<uint16_t, 256> data;
};

struct GRP{
	std::array<unsigned char, 256*192> data;
};

struct Resources{
	//should contain all required resources
	PRG prg;
	MEM mem;
	std::map<std::string, GRP> grp; //grp0-3 (ul does nothing, gpage does nothing)
	std::map<std::string, CHR> chr; //bgu0-3*2,bgf0*2,bgd0-1*2,spu0-7 (on ul),sps0-1*2,spd0-3 (on ul)
	std::map<std::string, SCR> scr;	//scu0-1 *2 //works with ul and bgpage
	std::map<std::string, COL> col; //col0-2 *2 //ul only
};

class Program{
	const std::vector<Token> tokens;
	std::vector<Token>::const_iterator current;
	std::stack<std::vector<Token>::const_iterator> calls;
	
public:
	Program(const std::vector<Token>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string& label);
};

std::vector<Token> tokenize(PRG&);
