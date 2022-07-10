#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "PTC2Console.hpp"
#include "Background.hpp"
#include "PanelKeyboard.hpp"
#include "Icon.hpp"
#include "IPTCObject.hpp"

#include <array>

class Panel : public IPTCObject {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;
	
	Evaluator& e;
	PTC2Console c;
	TileMap panel_bg;
	Icon icon;
//	TileMap func_keys;
//	std::vector<std::string> func_text;
//	std::vector<SpriteInfo> keys;
//	SpriteArray key_sp;
	PanelKeyboard keyboard;
	
	int pnltype; //OFF PNL KYA KYM KYK
	
	void update_keytext();
	
	void pnltype_(const Args&);
	void pnlstr_(const Args&);
	
public:
	Panel(Evaluator&, Resources&, Input& i);
	
	Panel() = delete;
	
	Panel(const Panel&) = delete;
	
	Panel& operator=(const Panel&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	void touch_keys(bool, int, int);
	
	TileMap& draw_panel();
	SpriteArray& draw_keyboard();
	TileMap& draw_funckeys();
	Icon& draw_icon();
	
	int panel_on() { return pnltype; }
	//TODO: override with previous keyboard type
	void panel_override(int type = 2) { pnltype = type; }
	PTC2Console& get_console() { return c; }
	
//	void draw(sf::RenderWindow&, sf::RenderStates&);
	int get_last_keycode();
	std::pair<int,int> get_last_xy();
	std::pair<int,int> get_keycode_xy(int keycode);
};
