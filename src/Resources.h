#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <map>
#include <utility>

#include "Vars.h"
#include "Evaluator.h"

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

typedef const std::vector<std::vector<Token>>& Args;
using cmd_type = std::function<void(const Args&)>;
typedef std::pair<Token, cmd_type> cmd_map;

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
	void call_cmd(Token, const std::vector<std::vector<Token>>&);

	Program(Evaluator&, const std::vector<Token>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string&);
	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	void run();
};

std::vector<Token> tokenize(PRG&);
