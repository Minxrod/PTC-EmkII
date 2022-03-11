#pragma once

#include "Vars.h"
#include "Evaluator.h"
#include "Console.h"
#include "Background.h"
#include "PanelKeyboard.h"

#include <array>

class Panel {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;
	
	Evaluator& e;
	Console c;
	TileMap panel_bg;
//	TileMap func_keys;
//	std::vector<std::string> func_text;
//	std::vector<SpriteInfo> keys;
//	SpriteArray key_sp;
	PanelKeyboard keyboard;
	
	int pnltype; //OFF PNL KYA KYM KYK
	
	void update_keytext();
	
	void pnltype_(const Args&);
	void pnlstr_(const Args&);
	
	void iconset_(const Args&);
	void iconclr_(const Args&);
	Var iconchk_(const Vals&);
	
public:
	Panel(Evaluator&, Resources&, Input& i);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void touch_keys(bool, int, int);
	
	TileMap& draw_panel();
	SpriteArray& draw_keyboard();
	TileMap& draw_funckeys();
//	void draw(sf::RenderWindow&, sf::RenderStates&);
	int get_last_keycode();
	std::pair<int,int> get_last_xy();
	std::pair<int,int> get_keycode_xy(int keycode);
};
