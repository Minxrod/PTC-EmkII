#include "Panel.h"

Panel::Panel(Evaluator& ev, Resources& r, Input& i) : e{ev}, c{ev, r.chr.at("BGF0L"), i}{
}

std::map<Token, cmd_type> Panel::get_cmds(){
	return std::map<Token, cmd_type>{
		std::pair("PNLTYPE"_TC, getfunc(this, &Panel::pnltype_)),
		std::pair("PNLSTR"_TC, getfunc(this, &Panel::pnlstr_)),
	};
}

void Panel::pnltype_(const Args& a){
	//PNLTYPE type
	String type = std::get<String>(e.evaluate(a[1]));
	pnltype = std::string("OFF PNL KYA KYM KYK").find(type)/4;
	
}

void Panel::pnlstr_(const Args& a){
	//PNLSTR x,y,"STR",c
	Number x = std::get<Number>(e.evaluate(a[1]));
	Number y = std::get<Number>(e.evaluate(a[2]));
	Var text = e.evaluate(a[3]);
	Number col = std::get<Number>(e.evaluate(a[4]));
	
	c.print(static_cast<int>(x), static_cast<int>(y), text, static_cast<int>(col));
}

void Panel::iconset_(const Args&){
	
}

void Panel::iconclr_(const Args&){
	
}

Var Panel::iconchk_(const Vals&){
	return Var(0);
}

std::map<Token, op_func> Panel::get_funcs(){
	return std::map<Token, op_func>{};
}

std::array<unsigned char, Panel::WIDTH*Panel::HEIGHT*4>& Panel::draw(){
	std::fill(image.begin(), image.end(), 0);
	
	return image;
}
