#pragma once

#include "Vars.h"
#include "Sprites.h"
#include "SpriteArray.h"
#include "TileMap.h"

class PanelKeyboard {
	TileMap func_keys;
	std::vector<std::string> func_text;
	std::vector<SpriteInfo> keys;
	SpriteArray key_sp;
	
	void update_keytext();
	
public:
	PanelKeyboard();
	void touch_keys(bool t, int x, int y);
	SpriteArray& draw_keyboard();
	TileMap& draw_funckeys();
};
