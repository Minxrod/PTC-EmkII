#include "Sprites.hpp"

#include <cmath>

Sprites::Sprites(Evaluator& ev) : e{ev}{
	page = 0;
	sprites = std::vector<std::vector<SpriteInfo>>{2, std::vector<SpriteInfo>{100, SpriteInfo{}}};
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
		std::pair("SPSETV"_TC, getfunc(this, &Sprites::spsetv_)),
		std::pair("SPREAD"_TC, getfunc(this, &Sprites::spread_)),
	};
}

std::map<Token, op_func> Sprites::get_funcs(){
	return std::map<Token, op_func>{
		std::pair<Token, op_func>("SPCHK"_TF, getfunc<Sprites>(this, &Sprites::spchk_)),
		std::pair<Token, op_func>("SPHIT"_TF, getfunc<Sprites>(this, &Sprites::sphit_)),
		std::pair<Token, op_func>("SPHITSP"_TF, getfunc<Sprites>(this, &Sprites::sphitsp_)),
		std::pair<Token, op_func>("SPGETV"_TF, getfunc<Sprites>(this, &Sprites::spgetv_)),
	};
}

/// PTC command to set the current sprite page.
/// 
/// Format: 
/// * `SPPAGE page`
/// 
/// Arguments:
/// * page: Current sprite page
/// 
/// @param a Arguments
void Sprites::sppage_(const Args& a){
	//SPPAGE page
	page = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

/// PTC command to initialize a sprite.
/// @note Does not currently check for valid (width,height) combinations.
/// 
/// Format: 
/// * `SPSET id,chr,pal,h,v,prio[,width,height]`
/// 
/// Arguments:
/// * id: Sprite id/control number
/// * chr: Character code [0-511]
/// * pal: Palette [0-15]
/// * h: Horizontal flip state
/// * v: Vertical flip state
/// * prio: Sprite draw priority
/// * width: Width of sprite (pixels)
/// * height: Height of sprite (pixels)
/// 
/// @param a Arguments
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
	s.pos.time = 0;
	s.anim.current_frame = 0;
	s.anim.loop_forever = false;
	s.anim.loop = 0;
	s.scale.time = 0;
	s.angle.time = 0;
}

/// PTC command to clear a sprite or sprites.
/// 
/// Format: 
/// * `SPCLR [id]`
/// 
/// Arguments:
/// * id: Control number of sprite to clear. If omitted, clears all sprites on current screen.
/// 
/// @param a Arguments
void Sprites::spclr_(const Args& a){
	//SPCLR [id]
	if (a.size() == 1){
		sprites[page] = std::vector<SpriteInfo>{100, SpriteInfo{}};
	} else {
		auto id = std::get<Number>(e.evaluate(a[1]));
		sprites[page][id] = SpriteInfo{};
	}
}

/// PTC command to set the sprite's home coordinates.
/// 
/// Format: 
/// * `SPHOME id,x,y`
/// 
/// Arguments:
/// * id: Control number of sprite
/// * x: Sprite home x
/// * y: Sprite home y
/// 
/// @param a Arguments
void Sprites::sphome_(const Args& a){
	//SPHOME id x y
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	s.home_x = std::get<Number>(e.evaluate(a[2]));
	s.home_y = std::get<Number>(e.evaluate(a[3]));
}

/// PTC command to change a sprite's position.
/// 
/// Format: 
/// * `SPOFS id,x,y[,time]`
/// 
/// Arguments:
/// * id: Control number of sprite
/// * x: New sprite x
/// * y: New sprite y
/// * time: Frames to move sprite for
/// 
/// @param a Arguments
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
		if (time){
			s.pos.dx = (x - s.pos.x) / time;
			s.pos.dy = (y - s.pos.y) / time;
			s.pos.time = time;
		} else {
			s.pos.x = x;
			s.pos.y = y;
			s.pos.time = -1;
		}
	} else {
		s.pos.x = std::get<Number>(e.evaluate(a[2]));
		s.pos.y = std::get<Number>(e.evaluate(a[3]));
		s.pos.time = -1;
	}
}

/// PTC command to set visual attributes of an already initialized sprite.
/// 
/// Format: 
/// * `SPCHR id,chr[,pal,h,v,prio]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * chr: Character code [0-511]
/// * pal: Palette [0-15]
/// * h: Horizontal flip state
/// * v: Vertical flip state
/// * prio: Sprite draw priority
/// 
/// @param a Arguments
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

/// PTC command to animate a sprite.
/// 
/// Format: 
/// * `SPANIM id,chrs,time[,loop]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * chr: Character code [0-511]
/// * time: Frames to animate sprite for
/// * loop: How many times to loop (default 0=forever)
/// 
/// @param a Arguments
void Sprites::spanim_(const Args& a){
	//SPANIM id number_frames time [loop]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto chrs = std::get<Number>(e.evaluate(a[2]));
	auto frames = std::get<Number>(e.evaluate(a[3]));
	auto loop = 0;
	if (a.size() >= 5)
		loop = std::get<Number>(e.evaluate(a[4]));
	
	s.anim.chrs = chrs;
	s.anim.frames_per_chr = frames;
	s.anim.loop = loop;
	s.anim.current_frame = 0;
	s.anim.current_chr = s.chr;
	s.anim.loop_forever = loop == 0;
}

/// PTC command to rotate a sprite.
/// 
/// Format: 
/// * `SPANGLE id,angle[,time[,dir]]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * angle: New sprite rotation (degrees)
/// * time: Time to move to new rotation (frames)
/// * dir: Direction (default 1=clockwise, -1=counterclockwise)
/// 
/// @param a Arguments
void Sprites::spangle_(const Args& a){
	//SPANGLE id angle [time] [direction]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	auto ang = -std::get<Number>(e.evaluate(a[2]));
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

/// PTC command to scale a sprite.
/// 
/// Format: 
/// * `SPSCALE id,scale[,time]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * scale: New sprite scale
/// * time: Time to change to new scale (frames)
/// 
/// @param a Arguments
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

/// PTC function to check the sprite's animation status.
/// 
/// Format: 
/// * `SPCHK(id)`
/// 
/// Arguments:
/// * id: Sprite control number
/// 
/// @param v Values
/// @return Bitfield corresponding to animation state
Var Sprites::spchk_(const Vals& v){
	//SPCHK id
	auto id = std::get<Number>(v.at(0));
	auto& s = sprites[page][id];
	
	int result = (int)(s.pos.time >= 0);
	result |= (int)(s.angle.time > 0) << 1;
	result |= (int)(s.scale.time > 0) << 2;
	result |= (int)(s.anim.loop_forever || s.anim.loop > 0) << 3;
	return Var((double)result);
}

/// PTC command to read the current state of a sprite.
/// 
/// Format: 
/// * `SPREAD(id),x[,y[,angle[,scale,[chr]]]]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * x: Current sprite x (`SPOFS`)
/// * y: Current sprite y (`SPOFS`)
/// * angle: Current sprite angle (`SPANGLE`)
/// * scale: Current sprite scale (`SPSCALE`)
/// * chr: Current sprite character code (`SPANIM`)
/// 
/// @param a Arguments
void Sprites::spread_(const Args& a){
	// SPREAD (id),x[,y[,angle[,scale[,chr]]]]
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	
	//tiny offset to avoid rounding errors from repeated addition of rounded values
	if (a.size() > 2)
		e.assign(a[2], Token{std::to_wstring(std::floor(s.pos.x + 1.0/8192)), Type::Num});
	if (a.size() > 3)
		e.assign(a[3], Token{std::to_wstring(std::floor(s.pos.y + 1.0/8192)), Type::Num});
	if (a.size() > 4)
		e.assign(a[4], Token{std::to_wstring(s.angle.a), Type::Num});
	if (a.size() > 5)
		e.assign(a[5], Token{std::to_wstring(s.scale.s), Type::Num});
	if (a.size() > 6){
		int current_chr = s.anim.loop_forever || s.anim.loop ? s.anim.current_chr : s.chr;
		e.assign(a[6], Token{std::to_wstring(current_chr), Type::Num});
	}
}

/// PTC command to set a sprite variable.
/// 
/// Format: 
/// * `SPSETV id,var,value`
/// 
/// Arguments:
/// * id: Sprite control number
/// * var: Variable id [0-7]
/// * value: Value to store
/// 
/// @param a Arguments
void Sprites::spsetv_(const Args& a){
	// SPSETV id var value
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	
	int var = std::get<Number>(e.evaluate(a[2]));
	auto value = std::get<Number>(e.evaluate(a[3]));
	
	s.vars[var] = value;
}

/// PTC function to get a sprite variable.
/// 
/// Format: 
/// * `SPGETV(id,var)`
/// 
/// Arguments:
/// * id: Sprite control number
/// * var: Variable id [0-7]
/// 
/// @param v Values
/// @return Value of sprite variable
Var Sprites::spgetv_(const Vals& v){
	// SPGETV(id, var)
	auto id = std::get<Number>(v[0]);
	auto& s = sprites[page][id];
	
	int var = std::get<Number>(v[1]);

	return s.vars[var];
}

/// PTC command to set the collision information for a sprite.
/// 
/// Format: 
/// * `SPCOL id,x,y,w,h,scale[,group]`
/// 
/// Arguments:
/// * id: Sprite control number
/// * x: Hitbox x
/// * y: Hitbox y
/// * w: Hitbox width
/// * h: Hitbox height
/// * scale: Adjust for `SPSCALE`?
/// * group: Collision mask
/// 
/// @param a Arguments
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

/// PTC command to set collision vectors as if the sprite was moving.
/// 
/// Format: 
/// * `SPCOLVEC id,dx,dy`
/// 
/// Arguments:
/// * id: Sprite control number
/// * dx: Displacement x
/// * dy: Displacement y
/// 
/// @param a Arguments
void Sprites::spcolvec_(const Args& a){
	//SPCOLVEC id dx dy
	auto id = std::get<Number>(e.evaluate(a[1]));
	auto& s = sprites[page][id];
	
	s.hit.dx = std::get<Number>(e.evaluate(a[2]));
	s.hit.dy = std::get<Number>(e.evaluate(a[3]));
}

/// PTC function to check sprite collision status.
/// @warning SPHITX, SPHITY, SPHITT are not implemented yet.
/// 
/// Format: 
/// * `SPHIT(id[,num])`
/// 
/// Arguments:
/// * id: Sprite control number
/// * num: Sprite number to start checking from
/// 
/// @param v Values
/// @return True if collision
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
	if (i == 100){
		return Var(0.0);
	} else {
		e.vars.write_sysvar("SPHITNO", (double)i);
		e.vars.write_sysvar("SPHITX", s.pos.x);
		e.vars.write_sysvar("SPHITY", s.pos.y);
		return Var(1.0);
//		e.vars.write_sysvar("SPHITT", i);
	}
}

/// PTC function to check sprite collision against another sprite.
/// @warning SPHITX, SPHITY, SPHITT are not implemented yet.
/// 
/// Format: 
/// * `SPHITSP(id,id2)`
/// 
/// Arguments:
/// * id: Sprite control number
/// * id2: Other sprite control number
/// 
/// @param v Values
/// @return True if collision between sprite id and id2
Var Sprites::sphitsp_(const Vals& v){
	//SPHITSP(id1 id2)
	auto id1 = std::get<Number>(v.at(0));
	auto& s1 = sprites[page][id1];
	auto id2 = std::get<Number>(v.at(1));
	auto& s2 = sprites[page][id2];
	
	//TODO: This apparently affects SPHITX,SPHITY,SPHITT, but not SPHITNO?
	//SPHITX,SPHITY = s1.pos.x, s1.pos.y
	e.vars.write_sysvar("SPHITX", s1.pos.x);
	e.vars.write_sysvar("SPHITY", s1.pos.y);
	//SPHITT = ???? what does this do??
	
	return Var((double)is_hit(s1, s2));
}

/// PTC function to check sprite collision with a rectangle.
/// @warning Unimplemented, always returns false
/// 
/// Format: 
/// * `SPHITRC(id,sx,sy,w,h[,dx,dy])`
/// 
/// Arguments:
/// * id: Sprite control number
/// * sx: Starting x
/// * sy: Starting y
/// * w: Rectangle width
/// * h: Rectangle height
/// * dx: Displacement x
/// * dy: Displacement y
/// 
/// @param v Values
/// @return True if collision between sprite and rectangle
Var Sprites::sphitrc_(const Vals&){
	
	return Var(0.0);
}

void Sprites::reset(){
	sprites[0] = std::vector<SpriteInfo>{100, SpriteInfo{}};
	sprites[1] = std::vector<SpriteInfo>{100, SpriteInfo{}};
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
				if (s.pos.time >= 0){
					s.pos.x += s.pos.dx;
					s.pos.y += s.pos.dy;
					s.pos.time--;
				}
				if (s.anim.loop_forever || s.anim.loop > 0){
					s.anim.current_frame++;
					
					if (s.anim.current_frame == s.anim.frames_per_chr){ //single frame end
						s.anim.current_frame = 0;
						int chr_step = std::max(s.w * s.h / 256, 1);
						s.anim.current_chr += chr_step;
						
						if ((s.anim.current_chr - s.chr) / chr_step == s.anim.chrs){ //loop complete
							s.anim.current_chr = s.chr;
							if (!s.anim.loop_forever)
								s.anim.loop--;
							if (!s.anim.loop_forever && s.anim.loop == 0){ //all loops complete
								s.chr = s.anim.current_chr; //no more animation; frame remains last frame
							}
//								s.anim.current_chr = s.chr; //reset loop,
						}
					}
				}
			}
		}
	}
}

SpriteArray Sprites::draw(int page, int prio){
	SpriteArray sa{};
	for (auto& spr : sprites[page]){
		if (spr.active && spr.prio == prio){
			if (spr.pos.y <= -spr.h + spr.home_y ||
			    spr.pos.x <= -spr.w + spr.home_x ||
			    spr.pos.y >= 192 + spr.home_y||
			    spr.pos.x >= 256 + spr.home_x){
				continue;
			}
			sa.add_sprite(spr);
		}
	}
	return sa;
}
