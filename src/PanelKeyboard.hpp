#pragma once

#include "Vars.hpp"
#include "Sprites.hpp"
#include "SpriteArray.hpp"
#include "TileMap.hpp"

class PanelKeyboard {
	TileMap func_keys;
	std::vector<std::string> func_text;
	std::vector<SpriteInfo> keys;
	
	std::vector<SpriteInfo*> space;
	std::vector<SpriteInfo*> run;
	std::vector<SpriteInfo*> edit;
	std::vector<std::vector<SpriteInfo*>> func;
	
	SpriteInfo* last_pressed;
	
	SpriteArray key_sp;
	
	void update_keytext();
	void update_key(SpriteInfo& key, int dir);
	
public:
	PanelKeyboard();
	PanelKeyboard(const PanelKeyboard&) = delete;
	PanelKeyboard& operator=(const PanelKeyboard&) = delete;
	
	void touch_keys(bool t, int x, int y);
	SpriteArray& draw_keyboard();
	TileMap& draw_funckeys();
	
	int get_last_keycode();
	std::pair<int, int> get_keycode_xy(int keycode);
	std::pair<int, int> get_last_xy();
};
