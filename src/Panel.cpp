#include "Panel.h"

Panel::Panel(Evaluator& ev, Resources& r, Input& i) : 
	e{ev},
	c{ev, r.chr.at("BGF0L"), i},
	panel_bg{64, 64},
	keyboard{}
{
	auto& key = r.scr.at("KEY").data;
	for (int y = 0; y < 24; ++y){
		for (int x=  0; x < 32; ++x){
			panel_bg.tile(x, y, key[2*(x+32*y)]);
		}
	}
	auto& pnl = r.scr.at("PNL").data;
	for (int y = 0; y < 24; ++y){
		for (int x=  0; x < 32; ++x){
			panel_bg.tile(x + 32, y, pnl[2*(x+32*y)]);
		}
	}
	pnltype = 2;
}

std::map<Token, cmd_type> Panel::get_cmds(){
	return std::map<Token, cmd_type>{
		std::pair("PNLTYPE"_TC, getfunc(this, &Panel::pnltype_)),
		std::pair("PNLSTR"_TC, getfunc(this, &Panel::pnlstr_)),
		std::pair("ICONCLR"_TC, getfunc(this, &Panel::iconset_)),
		std::pair("ICONSET"_TC, getfunc(this, &Panel::iconset_)),
	};
}

std::map<Token, op_func> Panel::get_funcs(){
	return std::map<Token, op_func>{
		std::pair("ICONCHK"_TF, getfunc(this, &Panel::iconchk_)),
	};
}

//void Panel::update_keytext(){
//	int x = 1;
//	for (auto& s : func_text){
//		auto str = s + "    ";
//		if (s.size() > 4){
//			str = s.substr(0,4) + ".";
//		} else {
//			str = str.substr(0,5);
//		}
//		for (char c : str){
//			func_keys.tile(x,0,c);
//			++x;
//		}
//		++x;
//	}
//}

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
	Number col = 0;
	if (a.size() == 5){
		col = std::get<Number>(e.evaluate(a[4]));
	}
	
	c.print(static_cast<int>(x), static_cast<int>(y), text, static_cast<int>(col));
}

void Panel::iconset_(const Args&){
	
}

void Panel::iconclr_(const Args&){
	
}

Var Panel::iconchk_(const Vals&){
	return Var(0.0);
}

void Panel::touch_keys(bool t, int x, int y){
	keyboard.touch_keys(t,x,y);
}

TileMap& Panel::draw_panel(){
	if (pnltype > 1) //kya kym kyk
		panel_bg.setPosition(0,192);
	else if (pnltype == 1) //pnl
		panel_bg.setPosition(256,192);
	else //off
		panel_bg.setPosition(0,192+256);
	return panel_bg;
}

SpriteArray& Panel::draw_keyboard(){
	SpriteArray& key_sp = keyboard.draw_keyboard();
	key_sp.setPosition(0,384);
	if (pnltype > 1){
		key_sp.setPosition(0,192);
	}
	return key_sp;
}

TileMap& Panel::draw_funckeys(){
	TileMap& func_keys = keyboard.draw_funckeys();
	if (pnltype > 1) //kya kym kyk
		func_keys.setPosition(0,192);
	else if (pnltype == 1) //pnl
		func_keys.setPosition(256,192);
	else //off
		func_keys.setPosition(0,192+256);
	return func_keys;
}

