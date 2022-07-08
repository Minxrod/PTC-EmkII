#include "PanelKeyboard.hpp"

const int SPACE_KEY_MIN = 320;
const int SPACE_KEY_MAX = 325;

const int FUNC_KEY_START = 330;

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
	space{}, run{}, edit{}, func{},
	last_pressed{nullptr},
	key_sp{}
{
	{
		int y = 6*8;
		int ch = 384;
		const int xofs[] = {3,0,3,4};
//		const char* str[] = {"1234567890-+=", "$\"QWERTYUIOP@*()", "!ASDFGHJKL;:<>", "'ZXCVBNM,./%"};
		int o = 0;
		int keycode = 2;
		for (int w : {13, 16, 14, 12}){
			for (int x = 0; x < w; ++x){
				auto k = create_key(16*x+8*xofs[o],y,ch);
				ch += 2;
				k.id = keycode;
				
				keys.push_back(k);
				key_sp.add_sprite(k);
				++keycode;
			}
			++keycode; //it's amazing this works lol
			y += 3*8;
			++o;
		}
	}
	//big keys (enter, shift, tab, etc.)
	{
		int x[]{0, 232, 0, 0, 224};
		int y[]{48, 48, 96, 120, 120};
		int ch[]{280, 276, 284, 296, 300};
		int keycodes[]{1, 15, 32, 47, 60};
		for (int i = 0; i < 5; ++i){
			auto k = create_big_key(x[i],y[i],ch[i],i>=3 ? 32 : 24);
			k.id = keycodes[i];
			
			keys.push_back(k);
			key_sp.add_sprite(k);
		}
	}
	//small keys (keyboard, caps lock, ins/del, etc.)
	{
		int x[]{0, 24, 40, 56, 200, 216, 240, 240};
		int ch[]{313, 508, 509, 510, 309, 310, 311, 315};
		int keycodes[]{61, 62, 63, 64, 66, 67, 68, 310};
		for (int i = 0; i < 8; ++i){
			auto k = create_small_key(x[i],i==7 ? 0 : 144, ch[i]);
			k.id = keycodes[i];
			
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
		space_l.id = SPACE_KEY_MIN;
		keys.push_back(space_l);
		key_sp.add_sprite(space_l);
		space.push_back(&keys.back());
		
		for (int i = 0; i < 4; ++i){
			auto space_m = create_small_key(112 + 16*i, 144, 495);
			space_m.id = SPACE_KEY_MIN + i + 1;
			keys.push_back(space_m);
			key_sp.add_sprite(space_m);
			space.push_back(&keys.back());
		}
		auto space_r = create_small_key(176,144,494);
		space_r.id = SPACE_KEY_MAX;
		keys.push_back(space_r);
		key_sp.add_sprite(space_r);
		space.push_back(&keys.back());
	}
	//function keys (FILES, LOAD", etc.)
	{
		for (int f = 0; f < 5; f++){
			auto func_l = create_small_key(f*48, -1, 505 - f);
			func_l.id = FUNC_KEY_START + f * 2;
			auto func_r = key_same(f*48 + 16, -1, 499);
			func_r.w = 32;
			func_r.h = 16;
			func_r.hit.w = 32;
			func_r.hit.h = 16;
			func_r.id = FUNC_KEY_START + f * 2 + 1;
			
			func.push_back(std::vector<SpriteInfo*>{});
			
			keys.push_back(func_l);
			key_sp.add_sprite(func_l);
			func.back().push_back(&keys.back());
			keys.push_back(func_r);
			key_sp.add_sprite(func_r);
			func.back().push_back(&keys.back());
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
		run.push_back(&keys.back());
		keys.push_back(run_stop_r);
		run.push_back(&keys.back());
		keys.push_back(edit_l);
		edit.push_back(&keys.back());
		keys.push_back(edit_r);
		edit.push_back(&keys.back());
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

void PanelKeyboard::update_key(SpriteInfo& key, int dir){
	if (SPACE_KEY_MIN <= key.id && key.id <= SPACE_KEY_MAX){
		for (auto* space_spr : space){
			key_sp.update_sprite_xy(*space_spr, dir, dir);
		}
	} else if (FUNC_KEY_START <= key.id && key.id < FUNC_KEY_START + 10) {
		int func_pressed = (key.id - FUNC_KEY_START) / 2;
		for (auto* func_spr : func[func_pressed]){
			key_sp.update_sprite_xy(*func_spr, dir, dir);
		}
	} else if (341 == key.id || 342 == key.id) {
		for (auto* run_spr : run){
			key_sp.update_sprite_xy(*run_spr, dir, dir);
		}
	} else if (343 == key.id || 344 == key.id) {
		for (auto* edit_spr : edit){
			key_sp.update_sprite_xy(*edit_spr, dir, dir);
		}
	} else {
		//regular single sprite key
		key_sp.update_sprite_xy(key, dir, dir);
	}
}

//Note: updates graphics AND stores info about key pressed
void PanelKeyboard::touch_keys(bool t, int x, int y){
	if (last_pressed)
		update_key(*last_pressed, -1);
	
	auto touch_sprite = make_touch_sprite(x, y);
	if (!t){ // no touch: reset all
		last_pressed = nullptr;
		last_pressed_timer.reset();
		return;
	} else if (t && last_pressed){ // touch and hold
		if (!is_hit(touch_sprite, *last_pressed)){
			 // moved away from held key -> release key
			last_pressed = nullptr;
		} else { 
			// still holding old key
			update_key(*last_pressed, 1);
			last_pressed_timer.advance();
		}
		return; // don't search for new keys
	} else if (last_pressed_timer.current_time > 0){ // was holding, but moved away from key (t && !last_pressed && timer is still active)
		return; // don't update anything until touch is released (!t case)
	} else {
		//press keyboard keys (touching, no previous hold, no previous timer)
		for (SpriteInfo& key : keys){
			if (is_hit(touch_sprite, key)){
				update_key(key, 1);
				last_pressed = &key;
				last_pressed_timer.advance();
				return;
			}
		}
		// no key pressed
		last_pressed_timer.reset();
		return;
	}
}

SpriteArray& PanelKeyboard::draw_keyboard(){
	return key_sp;
}

TileMap& PanelKeyboard::draw_funckeys(){
	return func_keys;
}

std::pair<int, int> PanelKeyboard::get_keycode_xy(int keycode){
	if (keycode == 65)
		keycode = SPACE_KEY_MIN + 2;
	for (SpriteInfo& key : keys){
		if (key.id == keycode)
			return std::pair<int,int>{key.pos.x+key.w/2, key.pos.y+key.h/2};
	}
	throw std::runtime_error{"oops, keycode doesn't exist"};
}

int PanelKeyboard::get_last_keycode(){
	if (!last_pressed || !last_pressed_timer.check())
		return 0;
	auto k = last_pressed->id;
	if (k < 69)
		return k;
	if (SPACE_KEY_MIN <= k && k <= SPACE_KEY_MAX)
		return 65; //space keycode
	return 0;
}

std::pair<int, int> PanelKeyboard::get_last_xy(){
	if (!last_pressed)
		return std::pair<int,int>(0,0);	
	return std::pair<int,int>{last_pressed->pos.x, last_pressed->pos.y};
}
