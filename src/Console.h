#pragma once

#include <array>

#include "Vars.h"
#include "Evaluator.h"
#include "Resources.h"

class Console {
	Evaluator& e;
	CHR& c;
	
	static const int WIDTH = 32;
	static const int HEIGHT = 24;
	
	char cur_fg_color = 0;
	char cur_bg_color = 0;
	char cur_x = 0;
	char cur_y = 0;
	char tabstep = 4;
	
	std::array<unsigned char, WIDTH*HEIGHT> bg_color;
	std::array<unsigned char, WIDTH*HEIGHT> fg_color;	
	std::array<unsigned char, WIDTH*HEIGHT> text;
	
	bool advance();
	void newline();
	void tab();
	void scroll();

	void print_(const Var&);
	std::vector<Token> input_common(const Args&);
	
	void cls_(const Args&);
	void print_(const Args&);
	void locate_(const Args&);
	void color_(const Args&);
	void input_(const Args&);
	void linput_(const Args&);
	Var chkchr_(const Vals&);
	
	std::array<unsigned char, WIDTH*HEIGHT*4*8*8> image;
	
public:
	Console(Evaluator&, CHR&);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	std::array<unsigned char, WIDTH*HEIGHT*4*8*8>& draw();
};
