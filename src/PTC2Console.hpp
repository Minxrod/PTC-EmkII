#pragma once

#include <array>

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Input.hpp"
#include "TileMap.hpp"
#include "BaseConsole.hpp"
#include "IPTCObject.hpp"

class PTCSystem;

const int PTC2_CONSOLE_WIDTH = 32;
const int PTC2_CONSOLE_HEIGHT = 24;

class PTC2Console : public BaseConsole<char, PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT>,
					public IPTCObject {
//	Visual* v;
	
//	Input& in;
	PTCSystem* system;
	Evaluator& e;
	TileMap tm;
	
	Number* csrx;
	Number* csry;
	Number* tabstep;
	
	bool inTheStupidCorner = false;
	
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
	PTC2Console(PTCSystem* s);
	
	PTC2Console() = delete;
	
	PTC2Console(const PTC2Console&) = delete;
	
	PTC2Console& operator=(const PTC2Console&) = delete;
	
	void reset();
	
	void print(int, int, Var&, int);
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	TileMap& draw();
};
