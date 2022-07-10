#include "Background.hpp"

///
/// Converts tile coordinates to a SCR index.
///
/// @param x Tile x coordinate
/// @param y Tile y coordinate
/// @return Index into a SCR data array.
///
int to_scr_coords(int x, int y){
	// limit to range [0,63]
	x = x & 0x3f;
	y = y & 0x3f;
	
	int cx = x / 32;
	int cy = y / 32;
	int tx = x % 32;
	int ty = y % 32;
	
	return 2 * (cx * 1024 + cy * 2048 + tx + ty * 32);
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
		cmd_map("BGCOPY"_TC, getfunc(this, &Background::bgcopy_)),
		cmd_map("BGREAD"_TC, getfunc(this, &Background::bgread_)),
	};
}

std::map<Token, op_func> Background::get_funcs(){
	return std::map<Token, op_func>{
		func_map("BGCHK"_TF, getfunc(this, &Background::bgchk_))
	};
}


/// Converts separated tile info into a single number.
/// 
/// @param tile CHR number of tile [0-1023]
/// @param h Horizontal flip
/// @param v Vertical flip
/// @param pal Palette of tile [0-15]
/// @return Screen data for tile
int to_data(int tile, bool h, bool v, int pal){
	return tile + (h ? 0x400 : 0) + (v ? 0x800 : 0) + (pal << 12);
}

/// Gets the CHR number from combined tiledata.
/// @param tiledata Screen data
/// @return Character code [0-1023]
int get_chr(int tiledata){
	return tiledata & 0x03ff;
}

/// Gets the horizontal flip state from combined tiledata.
/// @param tiledata Screen data
/// @return true if flipped horizontally
bool get_h(int tiledata){
	return (bool)(tiledata & 0x0400);
}

/// Gets the vertical flip state from combined tiledata.
/// @param tiledata Screen data
/// @return true if flipped vertically
bool get_v(int tiledata){
	return (bool)(tiledata & 0x0800);
}

/// Gets the palette number from combined tiledata.
/// @param tiledata Screen data
/// @return Palette [0-15]
int get_pal(int tiledata){
	return (tiledata & 0xf000) >> 12;
}

/// Places a tile on the background layer.
/// This function both writes to the renderable TileMap and the data format SCR.
/// 
/// @param s SCR to write tiledata to.
/// @param t TileMap to write tiledata to for rendering.
/// @param x x coordinate, in tiles.
/// @param y y coordinate, in tiles.
/// @param d Combined tiledata/screen data.
void place_tile(SCR& s, TileMap& t, int x, int y, int d){
	s.data[to_scr_coords(x,y)] = d & 0x00ff;
	s.data[to_scr_coords(x,y)+1] = (d & 0xff00) >> 8;
	
	t.tile(x,y,get_chr(d),get_h(d),get_v(d));
	t.palette(x,y,16*get_pal(d));
}

/// Gets a tile from the given SCR and location.
/// 
/// @param s SCR to read from
/// @param x x coordinate, in tiles.
/// @param y y coordinate, in tiles.
/// @return Screen data at (x,y).
int get_tile(SCR& s, int x, int y){
	return s.data[to_scr_coords(x,y)] | (s.data[to_scr_coords(x,y)+1] << 8);
}

/// PTC command to set the current BG page.
/// 
/// Format: `BGPAGE screen`
/// 
/// Arguments:
/// * screen: 0 for top screen, 1 for bottom screen
/// 
/// @param a Arguments
void Background::bgpage_(const Args& a){
	page = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

/// PTC command to clear the BG screen.
/// 
/// Format: `BGCLR [layer]`
/// 
/// Arguments:
/// * layer: Layer to clear. If omitted, clears both layers.
/// 
/// @param a Arguments
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

/// PTC command to set the BG rendering area.
/// 
/// Format: `BGCLIP x1,y1,x2,y2`
/// 
/// Area is from (x1,y1) to (x2,y2) inclusive.
/// 
/// @param a Arguments
void Background::bgclip_(const Args& a){
	//BGCLIP x1 y1 x2 y2
	bgclip_x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	bgclip_y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	bgclip_x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	bgclip_y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));

	//auto& fg = bg_layers.at(2*page+0);
	//auto& bg = bg_layers.at(2*page+1);
	
	//fg.clip(bgclip_x1,bgclip_y1,bgclip_x2,bgclip_y2);
	//bg.clip(bgclip_x1,bgclip_y1,bgclip_x2,bgclip_y2);
}

/// PTC command to set the offset of a BG layer. 
/// Optionally, can animate the position linearly over time.
/// 
/// Format: `BGOFS layer,x,y[,time]`
/// 
/// Arguments:
/// * layer: BG layer (foreground=0, background=1)
/// * x: x offset (pixels)
/// * y: y offset (pixels)
/// * time: Time taken to move to new position (in frames). If omitted, layer moves immediately.
/// 
/// @param a Arguments
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

/// PTC command to place a single BG tile.
/// 
/// Formats:
/// * `BGPUT layer,x,y,tile`
/// * `BGPUT layer,x,y,chr,pal,h,v`
/// 
/// Arguments:
/// * layer: Layer to place tile on.
/// * x: X coordinate in tiles.
/// * y: Y coordinate in tiles.
/// * tile: Screen data (combination of chr,pal,h,v)
/// * chr: CHR number.
/// * pal: Palette number.
/// * h: Horizontal flip state.
/// * v: Vertical flip state.
/// 
/// @param a Arguments
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
		auto tile = e.evaluate(a[4]);
		if (std::holds_alternative<Number>(tile)){
			tiledata = static_cast<int>(std::get<Number>(tile));
		} else {
			tiledata = static_cast<int>(std::stoi(std::get<String>(tile), nullptr, 16));
		}
	} else {
		auto t = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
		auto p = static_cast<int>(std::get<Number>(e.evaluate(a[5])));
		auto h = static_cast<int>(std::get<Number>(e.evaluate(a[6])));
		auto v = static_cast<int>(std::get<Number>(e.evaluate(a[7])));
		
		tiledata = to_data(t,(bool)h,(bool)v,p);
	}
	
	place_tile(scu, bgl, x, y, tiledata);
}

/// PTC command to fill a rectangular region with the same tile.
/// 
/// Format:
/// * `BGFILL layer,x1,y1,x2,y2,tile`
/// * `BGFILL layer,x1,y1,x2,y2,chr,pal,h,v`
/// 
/// Arguments:
/// * layer: Layer to fill tiles in.
/// * x1,y1,x2,y2: Coordinates defining the corners of the rectangle.
/// * tile: Combined character data, as defined by to_data
/// * chr: CHR number.
/// * pal: Palette number.
/// * h: Horizontal flip state.
/// * v: Vertical flip state.
/// 
/// @param a Arguments
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


#include <sstream>
#include <iomanip>
/// Convert a 16 bit number to hex.
/// 
/// @param num number to convert
/// @return Hex string of number
std::string u16_to_hex(int num)
{
	std::stringstream ss;
	ss << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << num;
	return ss.str();
}

/// PTC command to read a tile from the BG page
/// 
/// Format: 
/// * `BGREAD (layer,x,y),data`
/// * `BGREAD (layer,x,y),chr,pal,h,v`
/// 
/// Arguments:
/// * layer: Layer to fill tiles in
/// * x: x location to read from
/// * y: y location to read from
/// * data: Variable to write screen data to
/// * chr: Variable to write CHR number
/// * pal: Variable to write palette number
/// * h: Variable to write horizontal flip state
/// * v: Variable to write vertical flip state
/// 
/// @param a Arguments
void Background::bgread_(const Args& a){

	auto a_ = a[1]; //copy so args are not modified
	//remove parens
	a_.erase(a_.begin());
	a_.erase(a_.end()-1);
	
	auto args = split(a_);
	
	int layer = std::get<Number>(e.evaluate(args[0]));
	int x = std::get<Number>(e.evaluate(args[1]));
	int y = std::get<Number>(e.evaluate(args[2]));
	
	std::string name = "SCU";
	std::string screen = page ? "L" : "U";
	name += std::to_string(layer) + screen;
	auto& scu = scr.at(name);
	
	double data = get_tile(scu, x, y);
	if (a.size() == 3){
		if (std::holds_alternative<Number>(e.evaluate(a[2]))){
			e.assign(a[2], Token{std::to_string(data), Type::Num});
		} else {
			e.assign(a[2], Token{u16_to_hex(data), Type::Str});
		}
	} else if (a.size() == 6){
		e.assign(a[2], Token{std::to_string(get_chr(data)), Type::Num});
		e.assign(a[3], Token{std::to_string(get_pal(data)), Type::Num});
		e.assign(a[4], Token{std::to_string(get_h(data)), Type::Num});
		e.assign(a[5], Token{std::to_string(get_v(data)), Type::Num});
	} else {
		throw std::runtime_error{"Wrong number of arguments"};
	}
}

/// PTC command to copy from one region of the BG page to another.
/// 
/// Format: 
/// * `BGCOPY layer,sx1,sy1,sx2,sy2,tx,ty`
/// 
/// Arguments:
/// * layer: Layer to copy within
/// * sx1: Source location x1
/// * sy1: Source location y1
/// * sx2: Source location x2
/// * sy2: Source location y2
/// * tx: Destination location x
/// * ty: Destination location y
/// 
/// @param a Arguments
void Background::bgcopy_(const Args& a){
	// 
	int layer = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int source_x1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int source_y1 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int source_x2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	int source_y2 = static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	int dest_x = static_cast<int>(std::get<Number>(e.evaluate(a[6])));
	int dest_y = static_cast<int>(std::get<Number>(e.evaluate(a[7])));
	
	std::string name = "SCU";
	std::string screen = page ? "L" : "U";
	name += std::to_string(layer) + screen;
	auto& scu = scr.at(name);
	auto& bgl = bg_layers.at(2*page+layer);
	
	// temporary SCR + TileMap to perform intermediate copy to
	SCR temp_copy{};
	temp_copy.data.resize(SCR::SIZE);
	TileMap temp_tilemap{64,64};
	
	for (int t = 0; t < 2; ++t){
		auto& from_scu = !t ? scu : temp_copy;
		auto& to_scu = !t ? temp_copy : scu;
		auto& tilemap = !t ? temp_tilemap : bgl;
		
		for (int x = source_x1; x <= source_x2; ++x){
			for (int y = source_y1; y <= source_y2; ++y){
				int relative_x = x - source_x1;
				int relative_y = y - source_y1;
				
				int tile = get_tile(from_scu, !t ? x : dest_x + relative_x, !t ? y : dest_y	+ relative_y);
				place_tile(to_scu, tilemap, dest_x + relative_x, dest_y + relative_y, tile);
			}
		}
	}
}

/// PTC command to check the animation state of a BG layer.
/// 
/// Format: 
/// * `BGCHK(layer)`
/// 
/// Values:
/// * layer: Layer to check
/// 
/// @param v Values
/// @return 1 if layer is moving, otherwise 0
Var Background::bgchk_(const Vals& v){
	int layer = static_cast<int>(std::get<Number>(v.at(0)));
	
	return Number(bg_info[2*page+layer].time > 0);
}

void Background::update(){
	for (auto& b : bg_info){
		if (b.time > 0){
			b.x += b.xstep;
			b.y += b.ystep;
			b.time--;
		}
	}
}

void Background::reset(){
	auto oldpage = page;
	//BGCLR (all)
	page = 0;
	bgclr_({});
	page = 1;
	bgclr_({});
	page = oldpage;
	//BGCLIP 0,0,31,23 (both screens)
	bgclip_x1 = 0;
	bgclip_y1 = 0;
	bgclip_x2 = 31;
	bgclip_y2 = 23;
	//for (auto& l : bg_layers){
	//	l.clip(0,0,31,23);
	//}
	//BGOFS (all),0,0
	for (auto& i : bg_info){
		i.x = 0;
		i.y = 0;
		i.time = 0;
	}
}

TileMap& Background::draw(int screen, int layer){
	auto& bgl = bg_layers[2*screen+layer];
	bgl.setPosition(-(int)bg_info[2*screen+layer].x % (64*8), -(int)bg_info[2*screen+layer].y % (64*8));
	return bgl;
}


