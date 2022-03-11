#pragma once

#include "Vars.h"
#include "Sprites.h"
#include "SpriteArray.h"
#include "TileMap.h"

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
	void touch_keys(bool t, int x, int y);
	SpriteArray& draw_keyboard();
	TileMap& draw_funckeys();
};
