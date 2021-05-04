#include "Visual.h"
#include "Vars.h"

Visual::Visual(Evaluator& ev, Resources& r) :
	visible{true,true,true,true,true,true},
	e{ev},
	c{ev, r.chr.at("BGF0U")}
{}

std::map<Token, cmd_type> Visual::get_cmds(){
	auto cmds = std::map<Token, cmd_type>{
		cmd_map{"VISIBLE"_TC, getfunc(this, &Visual::visible_)},
	};
	
	cmds.merge(c.get_cmds());
	//cmds.merge(b.get_cmds());
	return cmds;
}

std::map<Token, op_func> Visual::get_funcs(){
	auto funcs = c.get_funcs();
	//funcs.merge(b.get_cmds());
	return funcs;
}

void Visual::visible_(const Args& a){
	for (int i = 0; i < 6; i++)
		visible[i] = (bool)(int)std::get<Number>(e.evaluate(a[i]));
}
