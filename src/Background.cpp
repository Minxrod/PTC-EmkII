#include "Background.h"

int to_scr_coords(int x, int y){
	int cx = x / 32;
	int cy = y / 32;
	int tx = x % 32;
	int ty = y % 32;
	
	return 2 * (cx * 1024 + cy * 2024 + tx + ty * 32);
}

Background::Background(Evaluator& ev, std::map<std::string, SCR>& s) : e{ev}, scr{s}{
	bg_info = std::vector<BGInfo>{4, BGInfo{0,0,0,0,0}};
	bg_layers = std::vector<TileMap>{4, TileMap(64,64)};
	page = 0;
}

std::map<Token, cmd_type> Background::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("BGPAGE"_TC, getfunc(this, &Background::bgpage_)),
		cmd_map("BGCLR"_TC, getfunc(this, &Background::bgclr_)),
		cmd_map("BGCLIP"_TC, getfunc(this, &Background::bgclip_)),
		cmd_map("BGOFS"_TC, getfunc(this, &Background::bgofs_)),
		cmd_map("BGFILL"_TC, getfunc(this, &Background::bgfill_)),
		cmd_map("BGPUT"_TC, getfunc(this, &Background::bgput_)),
	};
}

std::map<Token, op_func> Background::get_funcs(){
	return std::map<Token, op_func>{
		func_map("BGCHK"_TF, getfunc(this, &Background::bgchk_))
	};
}

int to_data(int tile, bool h, bool v, int pal){
	return tile + (h ? 0x400 : 0) + (v ? 0x800 : 0) + (pal << 12);
}

int get_chr(int tiledata){
	return tiledata & 0x03ff;
}

bool get_h(int tiledata){
	return (bool)(tiledata & 0x0400);
}

bool get_v(int tiledata){
	return (bool)(tiledata & 0x0800);
}

int get_pal(int tiledata){
	return (tiledata & 0xf000) >> 12;
}

void place_tile(SCR& s, TileMap& t, int x, int y, int d){
	s.data[to_scr_coords(x,y)] = d & 0x00ff;
	s.data[to_scr_coords(x,y)] = (d & 0xff00) >> 8;
	
	t.tile(x,y,get_chr(d),get_h(d),get_v(d));
	t.palette(x,y,16*get_pal(d));
}

void Background::bgpage_(const Args& a){
	//BGPAGE screen
	page = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

void Background::bgclr_(const Args& a){
	//BGCLR [layer]
	std::string name = "SCU";
	std::string screen = page ? "L" : "U";
	if (a.size() == 2){
		int layer = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
		name += std::to_string(layer) + screen;
		auto& bgl = bg_layers.at(2*page+layer);
		for (int x = 0; x < 64; ++x){
			for (int y = 0; y < 64; ++y){
				place_tile(scr.at(name), bgl, x, y, 0);
			}
		}
	} else {
		for (int l = 0; l < 2; ++l){
			auto& bgl = bg_layers.at(2*page+l);
			for (int x = 0; x < 64; ++x){
				for (int y = 0; y < 64; ++y){
					place_tile(scr.at(name+std::to_string(l)+screen), bgl, x, y, 0);
				}
			}
		}
	}
}

void Background::bgclip_(const Args& a){
	//BGCLIP x1 y1 x2 y2
	bgclip_x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	bgclip_y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	bgclip_x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	bgclip_y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));

	auto& fg = bg_layers.at(2*page+0);
	auto& bg = bg_layers.at(2*page+1);
	
	fg.clip(bgclip_x1,bgclip_y1,bgclip_x2,bgclip_y2);
	bg.clip(bgclip_x1,bgclip_y1,bgclip_x2,bgclip_y2);
}

void Background::bgofs_(const Args& a){
	//BGOFS layer x y [time]
	int layer = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	auto& info = bg_info[2*page+layer];

	if (a.size() == 4){
		info.x = std::get<Number>(e.evaluate(a[2]));
		info.y = std::get<Number>(e.evaluate(a[3]));
		info.time = 0;
	} else {
		double ox = std::get<Number>(e.evaluate(a[2]));
		double oy = std::get<Number>(e.evaluate(a[3]));
		info.time = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
		info.xstep = (ox - info.x) / info.time;
		info.ystep = (oy - info.y) / info.time;
	}
}

void Background::bgput_(const Args& a){
	//BGPUT layer x y tile
	//BGPUT layer x y chr pal h v
	std::string name = "SCU";
	std::string screen = page ? "L" : "U";
	int layer = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	name += std::to_string(layer) + screen;
	auto& scu = scr.at(name);
	auto& bgl = bg_layers.at(2*page+layer);
	
	auto x = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	auto y = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	
	int tiledata = 0;
	if (a.size() == 5){
		tiledata = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	} else {
		auto t = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
		auto p = static_cast<int>(std::get<Number>(e.evaluate(a[5])));
		auto h = static_cast<int>(std::get<Number>(e.evaluate(a[6])));
		auto v = static_cast<int>(std::get<Number>(e.evaluate(a[7])));
		
		tiledata = to_data(t,(bool)h,(bool)v,p);
	}
	
	place_tile(scu, bgl, x, y, tiledata);
}

void Background::bgfill_(const Args& a){
	//BGFILL layer x1 y1 x2 y2 tile
	std::string name = "SCU";
	std::string screen = page ? "L" : "U";
	int layer = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	name += std::to_string(layer) + screen;
	auto& scu = scr.at(name);
	auto& bgl = bg_layers.at(2*page+layer);
	
	auto x1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	auto y1 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	auto x2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	auto y2 = static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	
	int tiledata = 0;
	if (a.size() == 7){
		tiledata = static_cast<int>(std::get<Number>(e.evaluate(a[6])));
	} else {
		auto t = static_cast<int>(std::get<Number>(e.evaluate(a[6])));
		auto p = static_cast<int>(std::get<Number>(e.evaluate(a[7])));
		auto h = static_cast<int>(std::get<Number>(e.evaluate(a[8])));
		auto v = static_cast<int>(std::get<Number>(e.evaluate(a[9])));
		
		tiledata = to_data(t,(bool)h,(bool)v,p);
	}
	
	for (int x = x1; x <= x2; ++x){
		for (int y = y1; y <= y2; ++y){
			place_tile(scu, bgl, x, y, tiledata);
		}
	}
}

void Background::bgread_(const Args&){

}

void Background::bgcopy_(const Args&){

}

Var Background::bgchk_(const Vals& v){
	int layer = static_cast<int>(std::get<Number>(v.at(0)));
	
	return Var(bg_info[2*page+layer].time > 0);
}

TileMap& Background::draw(int screen, int layer){
	auto& bgl = bg_layers[2*screen+layer];
	bgl.setPosition(-(int)bg_info[2*screen+layer].x % (64*8), -(int)bg_info[2*screen+layer].y % (64*8));
	return bgl;
}


