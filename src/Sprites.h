#pragma once

#include "Vars.h"
#include "Evaluator.h"
#include "SpriteArray.h"

class Sprites {
	Evaluator& e;
	std::vector<std::vector<SpriteInfo>> sprites;
	int page;
	
	void sppage_(const Args&);
	void spset_(const Args&);
	void spclr_(const Args&);
	void sphome_(const Args&);
	
	void spofs_(const Args&);
	void spchr_(const Args&);
	void spanim_(const Args&);
	void spangle_(const Args&);
	void spscale_(const Args&);
	
	Var spchk_(const Vals&);
	void spread_(const Args&);
	void spsetv_(const Args&);
	Var spgetv_(const Vals&);
	
	void spcol_(const Args&);
	void spcolvec_(const Args&);
	Var sphit_(const Vals&);
	Var sphitsp_(const Vals&);
	Var sphitrc_(const Vals&);
	
public:
	Sprites(Evaluator&);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void update();
	SpriteArray draw(int, int);
};
