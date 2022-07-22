#include "Icon.hpp"

SpriteInfo pagebutton(int id, int chr){
	SpriteInfo s;
	s.active = true;
	s.id = id;
	s.chr = chr;
	s.pal = 0;
	s.w = 16;
	s.h = 16;
	s.hit.w = 16;
	s.hit.h = 12; //TODO: check this
	s.prio = 0;
	return s;
}

SpriteInfo iconbutton(int id, int chr){
	SpriteInfo s;
	s.active = false;
	s.id = id;
	s.chr = chr * 4;
	s.pal = 0;
	s.w = 32;
	s.h = 32;
	s.hit.w = 24;
	s.hit.h = 24;
	s.prio = 0;
	s.pos.x = 160 + 24 * id;
	s.pos.y = 168;
	return s;
}

/// PTC command to create or modify an icon.
/// 
/// Format: 
/// * `ICONSET icon,chr`
/// 
/// Arguments:
/// * icon: Icon id to set
/// * chr: Icon sprite [0-63]
/// 
/// @param a Arguments
void Icon::iconset_(const Args& a){
	//ICONSET icon,chrid
	int icon = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int chr = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	auto& s = sprites.at(icon);
	s.chr = chr * 4;
	s.active = true;
}

/// PTC command to clear an icon.
/// 
/// Format: 
/// * `ICONCLR [icon]`
/// 
/// Arguments:
/// * icon: Icon id to clear. (default=all)
/// 
/// @param a Arguments
void Icon::iconclr_(const Args& a){
	//ICONCLR [icon]
	if (a.size() == 2){
		int icon = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
		auto& s = sprites.at(icon);
		s.active = false;
	} else {
		for (auto& s : sprites){
			s.active = false;
		}
	}
}

/// PTC function to check if an icon is pressed.
/// 
/// Format: 
/// * `ICONCHK()`
/// 
/// @param v Values
/// @return id of icon pressed, or -1 if no icons pressed
Var Icon::iconchk_(const Vals&){
	//ICONCHK()
	if (last_pressed_timer.check())
		return Number(last_icon_pressed < -1 ? -1 : last_icon_pressed);
	return Number(-1);
}

Icon::Icon(Evaluator& eval) : e{eval}{
	sprites = std::vector<SpriteInfo>{
		iconbutton(0,0),
		iconbutton(1,1),
		iconbutton(2,2),
		iconbutton(3,3),
	};
	
	up = pagebutton(-2, 60+256); //use SPK 60 (appended to end of 256 SPD)
	up.pos.x = 144;
	up.pos.y = 168;
	down = pagebutton(-3, 61+256); //SPK 61
	down.pos.x = 144;
	down.pos.y = 180;
	
	iconpuse = std::get<Number*>(e.vars.get_var_ptr("ICONPUSE"));
	iconpage = std::get<Number*>(e.vars.get_var_ptr("ICONPAGE"));
	iconpmax = std::get<Number*>(e.vars.get_var_ptr("ICONPMAX"));
}

std::map<Token, cmd_type> Icon::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("ICONSET"_TC, getfunc<Icon>(this, &Icon::iconset_)),
		cmd_map("ICONCLR"_TC, getfunc<Icon>(this, &Icon::iconclr_ )),
	};
}

std::map<Token, op_func> Icon::get_funcs(){
	return std::map<Token, op_func>{
		func_map("ICONCHK"_TF, getfunc<Icon>(this, &Icon::iconchk_)),
	};
}

void Icon::update(bool t, int x, int y){
	auto touch_sprite = make_touch_sprite(x, y);
	if (!t){ // no touch: reset all
		last_icon_pressed = -1;
		last_pressed_timer.reset();
		return;
	} else if (t && last_icon_pressed != -1){ // touch and hold
		auto& s = last_icon_pressed > -1 ? sprites[last_icon_pressed] : 
		         (last_icon_pressed == up.id ? up : down);
		
		if (!is_hit(touch_sprite, s)){
			 // moved away from held icon -> release icon
			last_icon_pressed = -1;
		} else { 
			// still holding previous icon
			last_pressed_timer.advance();
		}
		return; // don't search for new icon
	} else if (last_pressed_timer.current_time > 0){ // was holding, but moved away from icon (t && !last_pressed && timer is still active)
		return; // don't update anything until touch is released (!t case)
	} else {
		//touch new icon (touching, no previous hold, no previous timer)
		for (auto& s : sprites){
			if (is_hit(s, touch_sprite)){
				last_icon_pressed = s.id;
				last_pressed_timer.advance();
				return;
			}
		}
		if (*iconpuse){
			//check collision with up/down keys
			if (is_hit(up, touch_sprite)){
				last_icon_pressed = up.id;
				*iconpage -= *iconpage > 0;
				return;
			} else if (is_hit(down, touch_sprite)){
				last_icon_pressed = down.id;
				*iconpage += *iconpage < *iconpmax;
				return;
			}
		}
		// none pressed
		last_icon_pressed = -1;
		last_pressed_timer.reset();
		return;
	}
	//deduplicate with PanelKeyboard?
}

void Icon::draw(sf::RenderTarget& target, sf::RenderStates rs) const {
	SpriteArray sa;
	if (*iconpuse){
		sa.add_sprite(up);
		sa.add_sprite(down);
		if (last_icon_pressed < -1){
			sa.update_sprite_xy(last_icon_pressed == up.id ? up : down, 1, 1);
		}
	}
	for (auto& si : sprites){
		if (si.active){
			sa.add_sprite(si);
			if (si.id == last_icon_pressed){
				sa.update_sprite_xy(si, 1, 1);
			}
		}
	}
	
	target.draw(sa, rs);
}
