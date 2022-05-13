#pragma once

#include <array>

#include "Vars.h"
#include "Evaluator.h"
#include "Resources.h"
#include "Input.h"
#include "TileMap.h"

class Console {
	Input& in;
	Evaluator& e;
	CHR& c;
	TileMap tm;
	
	static const int WIDTH = 32;
	static const int HEIGHT = 24;
	
	char cur_fg_color = 0;
	char cur_bg_color = 0;
	Number* cur_x;
	Number* cur_y;
	Number* tabstep;
	
	std::array<unsigned char, WIDTH*HEIGHT> bg_color;
	std::array<unsigned char, WIDTH*HEIGHT> fg_color;	
	std::array<unsigned char, WIDTH*HEIGHT> text;
	
	bool advance();
	void newline();
	void tab();
	void scroll();

	void print_(const Var&);
	void print_str(std::string);
	std::pair<std::vector<Token>, std::string> input_common(const Args&);
	
	void cls_(const Args&);
	void print_(const Args&);
	void locate_(const Args&);
	void color_(const Args&);
	void input_(const Args&);
	void linput_(const Args&);
	Var chkchr_(const Vals&);
	void ok(const Args&);
	
public:
	Console(Evaluator&, CHR&, Input&);
	
	void cls(const Args&);
	void reset();
	
	void print(int, int, Var&, int);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	TileMap& draw();
};
