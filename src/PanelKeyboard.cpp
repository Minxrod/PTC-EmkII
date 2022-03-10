#include "PanelKeyboard.h"

const int SPACE_KEY_MIN = 320;
const int SPACE_KEY_MAX = 325;

SpriteInfo key_same(int x, int y, int c){
	SpriteInfo k;
	k.active = true;
	k.pos.x = x;
	k.pos.y = y;
	k.chr = c;
	k.pal = 0;
	return k;
}

SpriteInfo create_key(int x, int y, int c){
	SpriteInfo k = key_same(x,y,c);
	k.w = 16;
	k.h = 32;
	k.hit.w = 16;
	k.hit.h = 24;
	return k;
}

SpriteInfo create_big_key(int x, int y, int c, int hw = 24){
	SpriteInfo k = key_same(x,y,c);
	k.w = 32;
	k.h = 32;
	k.hit.w = hw;
	k.hit.h = 24;
	return k;
}

SpriteInfo create_small_key(int x, int y, int c){
	SpriteInfo k = key_same(x,y,c);
	k.w = 16;
	k.h = 16;
	k.hit.w = 16;
	k.hit.h = 16;
	return k;
}

PanelKeyboard::PanelKeyboard() :
	func_keys{32, 1},
	func_text{"FILES","LOAD\"","SAVE\"","CONT","RUN"},
	keys{},
	key_sp{}
{
	{
		int y = 6*8;
		int ch = 384;
		const int xofs[] = {3,0,3,4};
		const char* str[] = {"1234567890-+=", "$\"QWERTYUIOP@*()", "!ASDFGHJKL;:<>", "'ZXCVBNM,./%"};
		int o = 0;
		for (int w : {13, 16, 14, 12}){
			for (int x = 0; x < w; ++x){
				auto k = create_key(16*x+8*xofs[o],y,ch);
				ch += 2;
				k.id = str[o][x];
				
				keys.push_back(k);
				key_sp.add_sprite(k);
			}
			y += 3*8;
			++o;
		}
	}
	//big keys (enter, shift, tab, etc.)
	{
		int x[]{0, 232, 0, 0, 224};
		int y[]{48, 48, 96, 120, 120};
		int ch[]{280, 276, 284, 296, 300};
		for (int i = 0; i < 5; ++i){
			auto k = create_big_key(x[i],y[i],ch[i],i>=3 ? 32 : 24);
			k.id = 300 + i;
			
			keys.push_back(k);
			key_sp.add_sprite(k);
		}
	}
	//small keys (keyboard, caps lock, ins/del, etc.)
	{
		int x[]{0, 24, 40, 56, 200, 216, 240, 240};
		int ch[]{313, 508, 509, 510, 309, 310, 311, 315};
		for (int i = 0; i < 8; ++i){
			auto k = create_small_key(x[i],i==7 ? 0 : 144, ch[i]);
			k.id = 310 + i;
			
			keys.push_back(k);
			key_sp.add_sprite(k);
		}
	}
	//long key (spacebar)
	{
		auto space_l = key_same(80,144,496);
		space_l.w = 32;
		space_l.h = 16;
		space_l.hit.w = 32;
		space_l.hit.h = 16;
		space_l.id = 320;
		keys.push_back(space_l);
		key_sp.add_sprite(space_l);

		for (int i = 0; i < 4; ++i){
			auto space_m = create_small_key(112 + 16*i, 144, 495);
			space_m.id = 321 + i;
			keys.push_back(space_m);
			key_sp.add_sprite(space_m);
		}
		auto space_r = create_small_key(176,144,494);
		space_r.id = 325;
		keys.push_back(space_r);
		key_sp.add_sprite(space_r);
	}
	//function keys (FILES, LOAD", etc.)
	{
		for (int f = 0; f < 5; f++){
			auto func_l = create_small_key(f*48, 0, 505 - f);
			func_l.id = 330 + f * 2;
			auto func_r = key_same(f*48 + 16, 0, 499);
			func_r.w = 32;
			func_r.h = 16;
			func_r.id = 330 + f * 2 + 1;
			
			keys.push_back(func_l);
			key_sp.add_sprite(func_l);
			keys.push_back(func_r);
			key_sp.add_sprite(func_r);
		}
	}
	//system keys(?) (RUN/EDIT, HELP)
	{
		auto help = create_big_key(0,168,304);
		help.id = 340;
		auto run_stop_l = create_big_key(40,168,256+12,32);
		run_stop_l.id = 341;
		auto run_stop_r = create_key(72,168,260+12);
		run_stop_r.id = 342;
		auto edit_l = create_key(88,168,262);
		edit_l.id = 343;
		edit_l.pal = 15;
		auto edit_r = create_big_key(104,168,264,32);
		edit_r.id = 344;
		edit_r.pal = 15;

		keys.push_back(help);
		keys.push_back(run_stop_l);
		keys.push_back(run_stop_r);
		keys.push_back(edit_l);
		keys.push_back(edit_r);
		key_sp.add_sprite(help);
		key_sp.add_sprite(run_stop_l);
		key_sp.add_sprite(run_stop_r);
		key_sp.add_sprite(edit_l);
		key_sp.add_sprite(edit_r);
	}
	update_keytext();
}

void PanelKeyboard::update_keytext(){
	int x = 1;
	for (auto& s : func_text){
		auto str = s + "    ";
		if (s.size() > 4){
			str = s.substr(0,4) + ".";
		} else {
			str = str.substr(0,5);
		}
		for (char c : str){
			func_keys.tile(x,0,c);
			++x;
		}
		++x;
	}
}

void PanelKeyboard::touch_keys(bool t, int x, int y){
	if (!t)
		return;
	//press keyboard keys
	SpriteInfo touch_sprite{};
	touch_sprite.active = true;
	touch_sprite.w = 1;
	touch_sprite.h = 1;
	touch_sprite.pos.x = x;
	touch_sprite.pos.y = y;
	touch_sprite.hit.x = 0;
	touch_sprite.hit.y = 0;
	touch_sprite.hit.w = 2;
	touch_sprite.hit.h = 2;
	for (SpriteInfo& key : keys){
//		if (key.id == 320 || is_hit(touch_sprite, key))
//			std::cout << key.pos.x << "," << key.pos.y << "," << is_hit(touch_sprite, key) << std::endl;
		
		if (is_hit(touch_sprite, key)){
			if (SPACE_KEY_MIN <= key.id && key.id <= SPACE_KEY_MAX){
//				for (SpriteInfo& key : keys){
					
//				}
			} else {
				//regular single sprite key
				key_sp.update_sprite_xy(key, 1, 1);
			}
			return;
		}
	}
	std::cout << x << "," << y << std::endl;
}

SpriteArray& PanelKeyboard::draw_keyboard(){
//	SpriteArray sa{};
//	key_sp.setPosition(0,384);
//	if (pnltype > 1){
//		key_sp.setPosition(0,192);
//	}
//	for (auto& k : keys){
//		sa.add_sprite(k);
//	}
	return key_sp;
}

TileMap& PanelKeyboard::draw_funckeys(){
//	if (pnltype > 1) //kya kym kyk
//		func_keys.setPosition(0,192);
//	else if (pnltype == 1) //pnl
///		func_keys.setPosition(256,192);
//	else //off
//		func_keys.setPosition(0,192+256);
	return func_keys;
}
