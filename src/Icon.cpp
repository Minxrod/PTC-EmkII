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

void Icon::iconset_(const Args& a){
	//ICONSET icon,chrid
	int icon = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int chr = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	auto& s = sprites.at(icon);
	s.chr = chr * 4;
	s.active = true;
}

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

Var Icon::iconchk_(const Vals&){
	//ICONCHK()
	return Number(last_icon_pressed);
}

Icon::Icon(Evaluator& eval) : e{eval}{
	sprites = std::vector<SpriteInfo>{
		iconbutton(0,0),
		iconbutton(1,1),
		iconbutton(2,2),
		iconbutton(3,3),
	};
	
	up = pagebutton(0, 60+256); //use SPK 60 (appended to end of 256 SPD)
	up.pos.x = 144;
	up.pos.y = 168;
	down = pagebutton(0, 61+256); //SPK 61
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

void Icon::update(bool touch, int x, int y){
	last_icon_pressed = -1;
	if (!touch)
		return;
	
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
	
	if (*iconpuse){
		//check collision with up/down keys
	}
	//check collision with icons
	//deduplicate with PanelKeyboard eventually
	
	for (auto& s : sprites){
		if (is_hit(s, touch_sprite)){
			last_icon_pressed = s.id;
		}
	}
}

void Icon::draw(sf::RenderTarget& target, sf::RenderStates rs) const {
	SpriteArray sa;
	if (*iconpuse){
		sa.add_sprite(up);
		sa.add_sprite(down);
	}
	for (auto& si : sprites){
		if (si.active)
			sa.add_sprite(si);
	}
	
	sa.setPosition(0,192);
	target.draw(sa, rs);
}
