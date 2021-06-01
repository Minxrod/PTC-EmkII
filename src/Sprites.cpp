#include "Sprites.h"

Sprites::Sprites(Evaluator& ev) : e{ev}{
	sprites = std::vector{2, std::vector{100, SpriteInfo{}}};
}

std::map<Token, cmd_type> Sprites::get_cmds(){
	return std::map<Token, cmd_type>{
		std::pair("SPPAGE"_TC, getfunc(this, &Sprites::sppage_)),
		std::pair("SPSET"_TC, getfunc(this, &Sprites::spset_)),
		std::pair("SPCLR"_TC, getfunc(this, &Sprites::spclr_)),
		std::pair("SPHOME"_TC, getfunc(this, &Sprites::sphome_)),
		std::pair("SPOFS"_TC, getfunc(this, &Sprites::spofs_)),
		std::pair("SPCHR"_TC, getfunc(this, &Sprites::spchr_)),
		std::pair("SPANIM"_TC, getfunc(this, &Sprites::spanim_)),
		std::pair("SPANGLE"_TC, getfunc(this, &Sprites::spangle_)),
		std::pair("SPSCALE"_TC, getfunc(this, &Sprites::spscale_)),
		std::pair("SPCOL"_TC, getfunc(this, &Sprites::spcol_)),
		std::pair("SPCOLVEC"_TC, getfunc(this, &Sprites::spcolvec_)),
	};
}

std::map<Token, op_func> Sprites::get_funcs(){
	return std::map<Token, op_func>{
		std::pair<Token, op_func>("SPCHK"_TF, getfunc<Sprites>(this, &Sprites::spchk_)),
		std::pair<Token, op_func>("SPHIT"_TF, getfunc<Sprites>(this, &Sprites::sphit_)),
		std::pair<Token, op_func>("SPHITSP"_TF, getfunc<Sprites>(this, &Sprites::sphitsp_)),
	};
}

void Sprites::sppage_(const Args& a){
	//SPPAGE page
	page = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

void Sprites::spset_(const Args& a){
	//SPSET id chr pal h v prio [width height]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto chr = std::get<Number>(e.evaluate(a[2]));
	auto pal = std::get<Number>(e.evaluate(a[3]));
	auto h = static_cast<bool>(std::get<Number>(e.evaluate(a[4])));
	auto v = static_cast<bool>(std::get<Number>(e.evaluate(a[5])));
	auto prio = std::get<Number>(e.evaluate(a[6]));
	double w = 16;
	double hi = 16;
	if (a.size() == 9){
		w = std::get<Number>(e.evaluate(a[7]));
		hi = std::get<Number>(e.evaluate(a[8]));
	}
	
	SpriteInfo& s = sprites[page][id];
	s = SpriteInfo{};
	s.active = true;
	s.id = id;
	s.chr = chr;
	s.pal = pal;
	s.flip_x = h;
	s.flip_y = v;
	s.prio = prio;
	s.w = w;
	s.h = hi;
	s.hit.x = 0;
	s.hit.y = 0;
	s.hit.w = s.w;
	s.hit.h = s.h;
}

void Sprites::spclr_(const Args& a){
	//SPCLR [id]
	if (a.size() == 1){
		sprites[page] = std::vector{100, SpriteInfo{}};
	} else {
		auto id = std::get<Number>(e.evaluate(a[1]));
		sprites[page][id] = SpriteInfo{};
	}
}

void Sprites::sphome_(const Args& a){
	//SPHOME id x y
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	s.home_x = std::get<Number>(e.evaluate(a[2]));
	s.home_y = std::get<Number>(e.evaluate(a[3]));
}
	
void Sprites::spofs_(const Args& a){
	//SPOFS id x y [time]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	if (a.size() == 5){
		double x, y;
		int time;
		x = std::get<Number>(e.evaluate(a[2]));
		y = std::get<Number>(e.evaluate(a[3]));
		time = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
		s.pos.dx = (x - s.pos.x) / time;
		s.pos.dy = (y - s.pos.y) / time;
		s.pos.time = time;
	} else {
		s.pos.x = std::get<Number>(e.evaluate(a[2]));
		s.pos.y = std::get<Number>(e.evaluate(a[3]));
		//may reset time? needs testing
	}
}

void Sprites::spchr_(const Args& a){
	//SPCHR id chr [pal h v prio]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto chr = std::get<Number>(e.evaluate(a[2]));
	auto pal = s.pal;
	auto h = s.flip_x;
	auto v = s.flip_y;
	auto prio = s.prio;
	if (a.size() == 7){
		pal = std::get<Number>(e.evaluate(a[3]));
		h = static_cast<bool>(std::get<Number>(e.evaluate(a[4])));
		v = static_cast<bool>(std::get<Number>(e.evaluate(a[5])));
		prio = std::get<Number>(e.evaluate(a[6]));
	}
	s.chr = chr;
	s.pal = pal;
	s.flip_x = h;
	s.flip_y = v;
	s.prio = prio;
}

void Sprites::spanim_(const Args& a){
	//SPANIM id number_frames time [loop]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto chrs = std::get<Number>(e.evaluate(a[2]));
	auto frames = std::get<Number>(e.evaluate(a[3]));
	auto loop = std::get<Number>(e.evaluate(a[4]));
	
	s.anim.chr = chrs;
	s.anim.frames = frames;
	s.anim.loop = loop;
	s.anim.time = frames;
}

void Sprites::spangle_(const Args& a){
	//SPANGLE id angle [time] [direction]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto ang = std::get<Number>(e.evaluate(a[2]));
	if (a.size() >= 4){
		auto time = std::get<Number>(e.evaluate(a[3]));
		if (a.size() == 5){
			auto dir = std::get<Number>(e.evaluate(a[4]));
			if (dir < 0){
				s.angle.da = (s.angle.a - ang) / time;
				s.angle.time = time;
			} else {
				s.angle.da = (ang - s.angle.a) / time;
				s.angle.time = time;
			}
		} else {
			s.angle.da = (ang - s.angle.a) / time;
			s.angle.time = time;
		}
	} else {
		s.angle.a = ang;
	}
}

void Sprites::spscale_(const Args& a){
	//SPSCALE id scale [time]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto scale = std::get<Number>(e.evaluate(a[2]));
	if (a.size() == 4){
		s.scale.time = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
		s.scale.ds = ((scale/100.0) - s.scale.s) / s.scale.time;
	} else {
		s.scale.s = (scale/100.0);
	}
}

Var Sprites::spchk_(const Vals& v){
	//SPCHK id
	auto id = std::get<Number>(v.at(0));
	auto& s = sprites[page][id];
	
	int result = (int)(s.pos.time > 0);
	result |= (int)(s.angle.time > 0) << 1;
	result |= (int)(s.scale.time > 0) << 2;
	result |= (int)(s.anim.time > 0) << 3;
	return Var((double)result);
}

void Sprites::spread_(const Args&){}

void Sprites::spsetv_(const Args&){}

Var Sprites::spgetv_(const Vals&){
	return Var(0.0);
}

void Sprites::spcol_(const Args& a){
	//SPCOL id x y w h scale [group]	
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	
	s.hit.x = std::get<Number>(e.evaluate(a[2]));
	s.hit.y = std::get<Number>(e.evaluate(a[3]));
	s.hit.w = std::get<Number>(e.evaluate(a[4]));
	s.hit.h = std::get<Number>(e.evaluate(a[5]));
	s.hit.scale_adjust = static_cast<bool>(std::get<Number>(e.evaluate(a[6])));
	if (a.size() == 8){
		s.hit.mask = static_cast<int>(std::get<Number>(e.evaluate(a[7])));
	} else {
		s.hit.mask = 0xff;
	}
}

void Sprites::spcolvec_(const Args& a){
	//SPCOLVEC id dx dy
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	
	s.hit.dx = std::get<Number>(e.evaluate(a[2]));
	s.hit.dy = std::get<Number>(e.evaluate(a[3]));
}

Var Sprites::sphit_(const Vals& v){
	//SPHIT(id [initial_number])
	auto id = std::get<Number>(v.at(0));
	auto& s = sprites[page][id];
	int start = 0;
	if (v.size() == 2){
		start = static_cast<int>(std::get<Number>(v.at(1)));
	}
	int i;
	for (i = start; i < 100; ++i){
		if (id == i)
			continue; //don't hit self
		if (is_hit(sprites[page][i], s)){
			break;
		}
	}
	if (i == 100)
		return Var(0.0);
	return Var(1.0);
}

Var Sprites::sphitsp_(const Vals& v){
	//SPHITSP(id1 id2)
	auto id1 = std::get<Number>(v.at(0));
	auto& s1 = sprites[page][id1];
	auto id2 = std::get<Number>(v.at(1));
	auto& s2 = sprites[page][id2];
	
	return Var((double)is_hit(s1, s2));
}

Var Sprites::sphitrc_(const Vals&){
	return Var(0.0);
}

void Sprites::update(){
	for (auto& sprpage : sprites){	
		for (auto& s : sprpage){
			if (s.active){
				if (s.scale.time > 0){
					s.scale.s += s.scale.ds;
					s.scale.time--;
				}
				if (s.angle.time > 0){
					s.angle.a += s.angle.da;
					s.angle.time--;
				}
				if (s.pos.time > 0){
					s.pos.x += s.pos.dx;
					s.pos.y += s.pos.dy;
					s.pos.time--;
				}
			}
		}
	}
}

SpriteArray Sprites::draw(int page, int prio){
	SpriteArray sa{};
	for (auto& spr : sprites[page]){
		if (spr.active && spr.prio == prio)
			sa.add_sprite(spr);
	}
	return sa;
}
