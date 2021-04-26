#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <map>
#include <utility>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"

struct PRG {
	std::vector<unsigned char> data;
};

struct CHR{
	static const int SIZE = 256*8*8/2;
	
	std::vector<unsigned char> data;
	
	CHR() = default;
	
	unsigned char get_pixel(int c, int x, int y){
		auto ch = data[32*c+x/2+4*y];
		return (x%2==0) ? (ch & 0x0f) : ((ch & 0xf0) >> 4);
	};
};

struct MEM{
	static const int SIZE = 256*2;
	
	std::vector<unsigned char> data;
	
	std::string get_mem();
};

struct SCR{
	static const int SIZE = 64*64*2;
	
	std::vector<unsigned char> data;
};

struct COL{
	static const int SIZE = 256*2;
	
	std::vector<unsigned char> data;
	
	std::vector<unsigned char> COL_to_RGBA(){
		std::vector<unsigned char> cols;
		cols.resize(256*4);
		
		for (int i = 0; i < 256; ++i){
			int col = (data[2*i] << 8) + data[2*i+1];
			uint8_t red = (col & 0x1F00) >> 8;
			uint8_t green = ((col & 0xE000) >> 13) | ((col & 0x0003) << 3);
			uint8_t blue = (col & 0x007C) >> 2;
			cols[4 * i] = 8 * red;
			cols[4 * i + 1] = 8 * green;
			cols[4 * i + 2] = 8 * blue;
			cols[4 * i + 3] = (i % 16 == 0) ? 0 : 255;
		}
		return cols;
	}
};

struct GRP{
	static const int SIZE = 256*192;
	
	std::vector<unsigned char> data;
};

struct Resources{
	//should contain all required resources
	Header prg_info;
	PRG prg;
	MEM mem;
	std::map<std::string, GRP> grp; //grp0-3 (ul does nothing, gpage does nothing)
	std::map<std::string, CHR> chr; //bgu0-3*2,bgf0*2,bgd0-1*2,spu0-7 (on ul),sps0-1*2,spd0-3 (on ul)
	std::map<std::string, SCR> scr;	//scu0-1 *2 //works with ul and bgpage
	std::map<std::string, COL> col; //col0-2 *2 //ul only
	
	Resources() = default;
	void load_program(std::string name);
	void load_default();
};

class Program{
	Evaluator& e;
	
	const std::vector<Token> tokens;
	std::vector<Token>::const_iterator current;
	
	std::stack<std::vector<Token>::const_iterator> gosub_calls;
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>> for_calls;

	std::map<Token, cmd_type> commands;
	
	void debugprint(const Args&);
	
	void if_(const Args&);
	void for_(const Args&);
	void next_(const Args&);
	void goto_(const Args&);
	void gosub_(const Args&);
	void on_(const Args&);
	void return_(const Args&);
	void end_(const Args&);
	void stop_(const Args&);
	void wait_(const Args&);
	
public:
	Program(Evaluator&, const std::vector<Token>&);
	
	void add_cmds(std::map<Token, cmd_type>);
	void call_cmd(Token, const std::vector<std::vector<Token>>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string&);
	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	void run();
};

std::vector<Token> tokenize(PRG&);
