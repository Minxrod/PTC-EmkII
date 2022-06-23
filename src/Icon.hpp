#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "SpriteArray.hpp"

class Icon {
	Evaluator& e;
	std::vector<SpriteInfo> sprites;
	
	//icon paging sysvars
	Number* iconpuse;
	Number* iconpmax;
	Number* iconpage;
	
	void iconset_(const Args&);
	void iconclr_(const Args&);
	Var iconchk_(const Vals&);
	
public:
	Icon(Evaluator& e);
	
	Icon() = delete;
	
	Icon(const Icon&) = delete;
	
	Icon& operator=(const Icon&) = delete;
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void update();
};
