#pragma once

#include "Vars.h"
#include "Evaluator.h"
#include "Console.h"

#include <array>

class Panel {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;
	
	Evaluator& e;
	Console c;
	//Background b;
	//Sprites s;
	
	int pnltype; //OFF PNL KYA KYM KYK
	
	void pnltype_(const Args&);
	void pnlstr_(const Args&);
	
	void iconset_(const Args&);
	void iconclr_(const Args&);
	Var iconchk_(const Vals&);
	
	std::array<unsigned char, WIDTH*HEIGHT*4> image;
	
public:
	Panel(Evaluator&, Resources&, Input& i);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	std::array<unsigned char, WIDTH*HEIGHT*4>& draw();
};
