#include "Icon.hpp"



void Icon::iconset_(const Args&){
	
}

void Icon::iconclr_(const Args&){
	
}

Var Icon::iconchk_(const Vals&){
	return Var(-1.0);
}

Icon::Icon(Evaluator& eval) : e{eval}{
	sprites = std::vector<SpriteInfo>(6);
	
	
	iconpuse = std::get<Number*>(e.vars.get_var_ptr("ICONPUSE"));
	iconpage = std::get<Number*>(e.vars.get_var_ptr("ICONPAGE"));
	iconpmax = std::get<Number*>(e.vars.get_var_ptr("ICONPMAX"));
}

std::map<Token, cmd_type> Icon::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("ICONSET"_TC, getfunc<Icon>(this, &Icon::iconset_)),
		cmd_map("ICONCLR"_TC, getfunc<Icon>(this, &Icon::iconclr_ )),
	};
}

std::map<Token, op_func> Icon::get_funcs(){
	return std::map<Token, op_func>{
		func_map("ICONCHK"_TF, getfunc<Icon>(this, &Icon::iconchk_)),
	};
}

void Icon::update(){
	if (*iconpuse){
		
	} else {
		
	}
}
