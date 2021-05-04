#pragma once

#include "Vars.h"
#include "Resources.h"
#include "Evaluator.h"
#include "Input.h"

#include "Console.h"
//#include "Background.h"
//#include "Sprites.h"
//#include "Graphics.h"

struct Visual {
	bool visible[6]; //CON, PNL, FG, BG, SP, GRP

	Evaluator& e;
	
	Console c;
	//Background b;
	//Sprites s;
	//Graphics g;
	
	void visible_(const Args&);
	
public:
	Visual(Evaluator&, Resources&, Input&);

	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
};
