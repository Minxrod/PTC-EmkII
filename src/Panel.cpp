#include "Panel.hpp"

#include "PTCSystem.hpp"

Panel::Panel(PTCSystem* s) : system{s},
	e{*system->get_evaluator()},
	c{system},
	panel_bg{64, 64},
	icon{e},
	keyboard{}
{
	Resources& r = *system->get_resources();
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
	auto cmds = std::map<Token, cmd_type>{
		std::pair("PNLTYPE"_TC, getfunc(this, &Panel::pnltype_)),
		std::pair("PNLSTR"_TC, getfunc(this, &Panel::pnlstr_)),
	};
	cmds.merge(icon.get_cmds());
	return cmds;
}

std::map<Token, op_func> Panel::get_funcs(){
	return icon.get_funcs();
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

/// PTC command to change the current panel type.
/// 
/// Format: 
/// * `PNLTYPE type`
/// 
/// Arguments:
/// * type: New type string (one of OFF, PNL, KYA, KYM, KYK)
/// 
/// @param a Arguments
void Panel::pnltype_(const Args& a){
	//PNLTYPE type
	String type = std::get<String>(e.evaluate(a[1]));
	pnltype = std::wstring(L"OFF PNL KYA KYM KYK").find(type)/4;
}

/// PTC command to print a string to the lower screen's console.
/// 
/// Format: 
/// * `PNLSTR x,y,str,c`
/// 
/// Arguments:
/// * x: X coordinate to print to
/// * y: Y coordinate to print to
/// * str: String to print (if it extends past the screen, str will be cut off)
/// * c: Color to use [0-15]
/// 
/// @param a Arguments
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

void Panel::touch_keys(bool t, int x, int y){
	if (pnltype > 1){
		keyboard.touch_keys(t,x,y);
	}
	icon.update(t,x,y);
}

TileMap& Panel::draw_panel(){
	if (pnltype > 1) //kya kym kyk
		panel_bg.setPosition(0,0);
	else if (pnltype == 1) //pnl
		panel_bg.setPosition(256,0);
	else //off
		panel_bg.setPosition(0,256);
	return panel_bg;
}

SpriteArray& Panel::draw_keyboard(){
	SpriteArray& key_sp = keyboard.draw_keyboard();
	key_sp.setPosition(0,192);
	if (pnltype > 1){
		key_sp.setPosition(0,0);
	}
	return key_sp;
}

TileMap& Panel::draw_funckeys(){
	TileMap& func_keys = keyboard.draw_funckeys();
	if (pnltype > 1) //kya kym kyk
		func_keys.setPosition(0,0);
	else if (pnltype == 1) //pnl
		func_keys.setPosition(256,0);
	else //off
		func_keys.setPosition(0,256);
	return func_keys;
}

Icon& Panel::draw_icon(){
	return icon;
}

int Panel::get_last_keycode(){
	return keyboard.get_last_keycode();
}

std::pair<int,int> Panel::get_last_xy(){
	return keyboard.get_last_xy();
}

std::pair<int,int> Panel::get_keycode_xy(int keycode){
	return keyboard.get_keycode_xy(keycode);
}

