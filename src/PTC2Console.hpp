#pragma once

#include <array>

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Input.hpp"
#include "TileMap.hpp"
#include "BaseConsole.hpp"
#include "IPTCObject.hpp"

const int PTC2_CONSOLE_WIDTH = 32;
const int PTC2_CONSOLE_HEIGHT = 24;

class PTC2Console : public BaseConsole<char, PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT>,
					public IPTCObject {
	Input& in;
	Evaluator& e;
	CHR& c;
	TileMap tm;
	
	Number* csrx;
	Number* csry;
	Number* tabstep;
	
	bool inTheStupidCorner = false;
//	BaseConsole<char, WIDTH, HEIGHT> internal_console;
	
/*	char cur_fg_color = 0;*/
/*	char cur_bg_color = 0;*/
/*	Number* cur_x;*/
/*	Number* cur_y;*/
/*	Number* tabstep;*/
	
/*	std::array<unsigned char, WIDTH*HEIGHT> bg_color;*/
/*	std::array<unsigned char, WIDTH*HEIGHT> fg_color;*/
/*	std::array<unsigned char, WIDTH*HEIGHT> text;*/
	
	//These are also methods in BaseConsole.
	//These are not proper overrides, but they are used as extensions of
	//the methodss from BaseConsole.
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
	PTC2Console(Evaluator&, CHR&, Input&);
	
	PTC2Console() = delete;
	
	PTC2Console(const PTC2Console&) = delete;
	
	PTC2Console& operator=(const PTC2Console&) = delete;
	
	void reset();
	
	void print(int, int, Var&, int);
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	TileMap& draw();
};
