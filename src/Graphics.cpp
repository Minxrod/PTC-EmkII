#include "Graphics.hpp"
#include "Visual.hpp"

#include <cmath>
#include <stack>
#include <set>

Graphics::Graphics(Evaluator& ev, std::map<std::string, GRP>& grps, Resources& res, Visual* vis) : 
	e{ev},
	r{res},
	v{vis},
	grp{grps},
	drawpage{0, 1},
	displaypage{0, 1},
	prio{3, 3}
{
	for (auto& i : image){
		std::fill(i.begin(), i.end(), 0);
	}
}

std::map<Token, cmd_type> Graphics::get_cmds(){
	return std::map<Token, cmd_type>{
		std::pair<Token, cmd_type>("GPAGE"_TC, getfunc(this, &Graphics::gpage_)),
		std::pair<Token, cmd_type>("GCLS"_TC, getfunc(this, &Graphics::gcls_)),
		std::pair<Token, cmd_type>("GCOLOR"_TC, getfunc(this, &Graphics::gcolor_)),
		std::pair<Token, cmd_type>("GPSET"_TC, getfunc(this, &Graphics::gpset_)),
		std::pair<Token, cmd_type>("GLINE"_TC, getfunc(this, &Graphics::gline_)),
		std::pair<Token, cmd_type>("GBOX"_TC, getfunc(this, &Graphics::gbox_)),
		std::pair<Token, cmd_type>("GFILL"_TC, getfunc(this, &Graphics::gfill_)),
		std::pair<Token, cmd_type>("GCIRCLE"_TC, getfunc(this, &Graphics::gcircle_)),
		std::pair<Token, cmd_type>("GDRAWMD"_TC, getfunc(this, &Graphics::gdrawmd_)),
		std::pair<Token, cmd_type>("GPUTCHR"_TC, getfunc(this, &Graphics::gputchr_)),
		std::pair<Token, cmd_type>("GPRIO"_TC, getfunc(this, &Graphics::gprio_)),
		std::pair<Token, cmd_type>("GCOPY"_TC, getfunc(this, &Graphics::gcopy_)),
		std::pair<Token, cmd_type>("GPAINT"_TC, getfunc(this, &Graphics::gpaint_)),
	};
}

std::map<Token, op_func> Graphics::get_funcs(){
	return std::map<Token, op_func>{
		std::pair<Token, op_func>("GSPOIT"_TF, getfunc(this, &Graphics::gspoit_)),
	};
}

int to_chr_coords(const int x, const int y){
	int tx = x % 8;
	int ty = y % 8;
	int cx = (x / 8) % 8;
	int cy = (y / 8) % 8;
	int bx = x / 64;
	int by = y / 64;
	
	return tx + 8 * ty + cx * 64 + cy * 512 + bx * 4096 + by * 4096*4;
}

void Graphics::draw_pixel(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x, const int y, const int c){
	if (x >= 0 && y >= 0 && x < 256 && y < 192){
		if (gdrawmd){ //XOR drawing on or off
			g.at(to_chr_coords(x,y)) ^= c;
			i.at(4*(x+256*y)) ^= c;
		} else {
			g.at(to_chr_coords(x,y)) = c;
			i.at(4*(x+256*y)) = c;
		}
	}
}

/// PTC command to set the default graphics drawing color
/// 
/// Format: 
/// * `GCOLOR color`
/// 
/// Arguments:
/// * color: The color to set as default [0-255]
/// 
/// @param a Arguments
void Graphics::gcolor_(const Args& a){
	//GCOLOR color
	gcolor = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

/// PTC command to clear the graphics screen.
/// 
/// Format: 
/// * `GCLS [color]`
/// 
/// Arguments:
/// * color: Color to clear screen with (defaults to gcolor)
/// 
/// @param a Arguments
void Graphics::gcls_(const Args& a){
	//GCLS [color]
	auto col = a.size() == 1 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	for (int y = 0; y < HEIGHT; ++y){
		for (int x = 0; x < WIDTH; ++x){
			draw_pixel(image[drawpage[screen]], g, x, y, col);
		}
	}
}

/// PTC command to set the current graphics page. Can optionally select specific drawing and display pages.
/// 
/// Format: 
/// * `GPAGE screen[,draw,disp]`
/// 
/// Arguments:
/// * screen: 0=upper screen, 1=lower screen
/// * draw: Draw page for selected screen [0-3]
/// * disp: Display page for selected screen [0-3]
/// 
/// @param a Arguments
void Graphics::gpage_(const Args& a){
	//GPAGE screen
	if (a.size() == 2){
		screen = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	} else { //GPAGE screen draw disp
		screen = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
		drawpage[screen] = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
		displaypage[screen] = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	}
}

/// PTC command to draw a pixel to the graphics page.
/// 
/// Format: 
/// * `GPSET x,y[,color]`
/// 
/// Arguments:
/// * x: x coordinate
/// * y: y coordinate
/// * color: Color to set pixel to (defaults to gcolor)
/// 
/// @param a Arguments
void Graphics::gpset_(const Args& a){
	//GPSET X,Y[,C]
	auto col = a.size() == 3 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	draw_pixel(image[drawpage[screen]],g,x,y,col);
}

void Graphics::draw_line(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x1, const int y1, const int x2, const int y2, const int c){
	if (x2==x1){
		//vertical line
		int dir_y = y2>y1 ? 1 : -1;
		for (int y = y1; y != y2; y+=dir_y){
			draw_pixel(i,g,x1,y,c);
		}
	} else {
		int dir_x = x2>x1 ? 1 : -1;
		double m = static_cast<double>(y2-y1)/(static_cast<double>(x1-x2)*(dir_x));
		draw_pixel(i,g,x1,y1,c);
		
		double y = y1;
		double ystep = 0;
		for(int x = x1; x != x2; x += dir_x){
			ystep += m;
			int ystepdir = ystep>0 ? -1 : 1;
			if (ystep >= 1 || ystep <= -1){
				for (int yp = static_cast<int>(ystep); yp != 0; yp+=ystepdir){
					draw_pixel(i,g,x,y,c);
					y+=ystepdir;
					ystep+=ystepdir;
				}
			} else {
				draw_pixel(i,g,x,y,c);
			}
		}
		int dir_y = y2>y1 ? 1 : -1;
		for ( ; y != y2; y+=dir_y){
			draw_pixel(i,g,x2,y,c);
		}
	}
}

/// PTC command to draw a line to the graphics page.
/// 
/// Format: 
/// * `GLINE x1,y1,x2,y2[,color]`
/// 
/// Arguments:
/// * x1: Starting point x
/// * y1: Starting point y
/// * x2: Ending point x
/// * y2: Ending point y
/// * color: Color of line (default = gcolor)
/// 
/// @param a Arguments
void Graphics::gline_(const Args& a){
	//GLINE x1 y1 x2 y2 [c]
	auto col = a.size() == 5 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	int x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;

	draw_line(image[drawpage[screen]],g,x1,y1,x2,y2,col);
}

/// PTC command to draw a circle to the graphics page.
/// @note Not perfectly accurate to PTC `GCIRCLE`.
/// 
/// Format: 
/// * `GCIRCLE x,y,r[,color]`
/// 
/// Arguments:
/// * x: Center of circle x coordinate
/// * y: Center of circle y coordinate
/// * r: Radius of circle
/// * color: Color of circle (default = gcolor)
/// 
/// @param a Arguments
void Graphics::gcircle_(const Args& a){
	//GCIRCLE x y r [c]
	auto col = a.size() == 4 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int r = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	const double detail = 64;
	for (int i = 0; i < detail; ++i){
		int x1 = x + r * std::cos(i / detail * 6.283);
		int y1 = y + r * std::sin(i / detail * 6.283);
		int x2 = x + r * std::cos((i + 1) / detail * 6.283);
		int y2 = y + r * std::sin((i + 1) / detail * 6.283);
		
		draw_line(image[drawpage[screen]],g,x1,y1,x2,y2,col);
	}
}

/// PTC command to flood fill a region starting from a point.
/// @note Does not attempt to implement the same bugs that `GPAINT` has in PTC.
/// 
/// Format: 
/// * `GPAINT x,y[,color]`
/// 
/// Arguments:
/// * x: Starting x
/// * y: Starting y
/// * color: Color to fill (default = gcolor)
/// 
/// @param a Arguments
void Graphics::gpaint_(const Args& a){
	//GPAINT x y [c]
	auto col = a.size() == 3 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	auto& i = image[drawpage[screen]];
	
	auto pixel = [i](auto pos, int xofs, int yofs) -> int{
		if (pos.first + xofs < 0 || pos.first + xofs > 255 || pos.second + yofs < 0 || pos.second + yofs > 191)
			return -1;
		return i[4*(pos.first+xofs+256*(pos.second+yofs))];
	};
	
	std::stack<std::pair<int, int>> oldpos;
	std::set<std::pair<int, int>> pos_added;
	oldpos.push({x,y});
	pos_added.insert({x,y});
	
	auto replace = pixel(oldpos.top(), 0, 0);
	if (replace == col){
		return;
	}
	
	while (!oldpos.empty()){
		auto pos = oldpos.top();
		oldpos.pop();
		
		draw_pixel(i,g,pos.first,pos.second,col);
		
		if (pixel(pos, 1, 0) == replace){
			std::pair<int,int> p = {pos.first+1, pos.second};
			if (!pos_added.count(p)){
				oldpos.push(p);
				pos_added.insert(p);
			}
		}
		if (pixel(pos, 0, 1) == replace){
			std::pair<int,int> p = {pos.first, pos.second+1};
			if (!pos_added.count(p)){
				oldpos.push(p);
				pos_added.insert(p);
			}
		}
		if (pixel(pos, -1, 0) == replace){
			std::pair<int,int> p = {pos.first-1, pos.second};
			if (!pos_added.count(p)){
				oldpos.push(p);
				pos_added.insert(p);
			}
		}
		if (pixel(pos, 0, -1) == replace){
			std::pair<int,int> p = {pos.first, pos.second-1};
			if (!pos_added.count(p)){
				oldpos.push(p);
				pos_added.insert(p);
			}
		}
	}
}

/// PTC command to draw an unfilled box.
/// 
/// Format: 
/// * `GBOX x1,y1,x2,y2[,color]`
/// 
/// Arguments:
/// * x1: Corner 1 x
/// * y1: Corner 1 y
/// * x2: Corner 2 x
/// * y2: Corner 2 y
/// * color: Color of box (default = gcolor)
/// 
/// @param a Arguments
void Graphics::gbox_(const Args& a){
	//GBOX x1 y1 x2 y2 [c]
	auto col = a.size() == 5 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	int x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	int bx = std::min(x1,x2);
	int by = std::min(y1,y2);
	int ex = std::max(x1,x2);
	int ey = std::max(y1,y2);

	for (int x = bx; x <= ex; ++x){
		for (int y = by; y <= ey; ++y){
			if (x == bx || x == ex || y == by || y == ey){
				draw_pixel(image[drawpage[screen]],g,x,y,col);
			}
		}
	}
}

/// PTC command to draw a filled box.
/// 
/// Format: 
/// * `GFILL x1,y1,x2,y2[,color]`
/// 
/// Arguments:
/// * x1: Corner 1 x
/// * y1: Corner 1 y
/// * x2: Corner 2 x
/// * y2: Corner 2 y
/// * color: Color of box (default = gcolor)
/// 
/// @param a Arguments
void Graphics::gfill_(const Args& a){
	//GFILL x1 y1 x2 y2 [c]
	auto col = a.size() == 5 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	int x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	int bx = std::min(x1,x2);
	int by = std::min(y1,y2);
	int ex = std::max(x1,x2);
	int ey = std::max(y1,y2);

	for (int x = bx; x <= ex; ++x){
		for (int y = by; y <= ey; ++y){
			draw_pixel(image[drawpage[screen]],g,x,y,col);
		}
	}
}

/// PTC command to draw a character to the graphics screen.
/// @note: This overwrites part of the graphics palette with whatever palette is used by the character drawn.
/// 
/// Format: 
/// * `GPUTCHR x,y,bank,chr,pal,scale`
/// 
/// Arguments:
/// * x: X coordinate (upper left)
/// * y: Y coordinate (upper left)
/// * bank: Character bank (ex. BGU0, SPU3, etc.)
/// * chr: Character code [0-255]
/// * pal: Palette [0-15]
/// * scale: Scale [1,2,4,8]
/// 
/// @param a Arguments
void Graphics::gputchr_(const Args& a){
	// GPUTCHR x y bank chr pal scale
	int x = std::get<Number>(e.evaluate(a[1]));
	int y = std::get<Number>(e.evaluate(a[2]));
	auto bank = std::get<String>(e.evaluate(a[3]));
	int chr = std::get<Number>(e.evaluate(a[4]));
	int pal = std::get<Number>(e.evaluate(a[5]));
	int scale = std::get<Number>(e.evaluate(a[6]));
	if (scale != 1 && scale != 2 && scale != 4 && scale != 8){
		throw std::runtime_error{"GPUTCHR invalid scale! (Must be in [1,2,4,8])"};
	}
	
	bank = r.normalize_type(bank);
	
	for (int i = pal * 16; i < pal * 16 + 16; ++i){
		auto& col2 = r.col.at(screen ? "COL2L" : "COL2U");
		auto& other = r.col.at(std::string(bank[0] == 'B' ? "COL0" : "COL1") + (screen ? "L" : "U"));
		col2.set_col(i, other.get_col(i));
	}
	v->regen_col();
	
	auto& chr_bank = r.chr.at(bank);
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	for (int px = 0; px < 8; ++px){
		for (int py = 0; py < 8; ++py){
			auto chr_c = chr_bank.get_pixel(chr, px, py);
			for (int sx = 0; sx < scale; ++sx){
				for (int sy = 0; sy < scale; ++sy){
					if (chr_c)
						draw_pixel(image[drawpage[screen]],g,x+px*scale+sx,y+py*scale+sy,chr_c ? chr_c + pal * 16 : 0);
				}
			}
		}
	}
}

/// PTC command to set the drawing mode.
/// 
/// Format: 
/// * `GDRAWMD state`
/// 
/// Arguments:
/// * state: true=XOR drawing mode, false=overwrite drawing mode
/// 
/// @param a Arguments
void Graphics::gdrawmd_(const Args& a){
	//GDRAWMD <status>
	gdrawmd = std::get<Number>(e.evaluate(a[1]));
}

/// PTC command to set the graphics priority.
/// 
/// Format: 
/// * `GPRIO priority`
/// 
/// Arguments:
/// * priority: Priority of current screen's graphics
/// 
/// @param a Arguments
void Graphics::gprio_(const Args& a){
	// GPRIO priority
	int p = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	prio[screen] = p;
}

/// PTC command to copy graphics from a rectangular region.
/// 
/// Format: 
/// * `GCOPY [source,]sx1,sy1,sx2,sy2,dx,dy,mode`
/// 
/// Arguments:
/// * source: Source graphics page. (default = current drawing page)
/// * sx1: Source corner 1 x
/// * sy1: Source corner 1 y
/// * sx2: Source corner 2 x
/// * sy2: Source corner 2 y
/// * dx: Destination x (upper left)
/// * dy: Destination y (upper left)
/// * mode: If true, copies color zero.
/// 
/// @param a Arguments
void Graphics::gcopy_(const Args& a){
	//GCOPY [sourcepage,]sx,sy,ex,ey,tx,ty,mode
	int page_offset = a.size() == 9 ? 1 : 0;
	int page = page_offset ? static_cast<int>(std::get<Number>(e.evaluate(a[1]))) : drawpage[screen];
	int source_x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1+page_offset])));
	int source_y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2+page_offset])));
	int source_x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3+page_offset])));
	int source_y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4+page_offset])));
	int dest_x = static_cast<int>(std::get<Number>(e.evaluate(a[5+page_offset])));
	int dest_y = static_cast<int>(std::get<Number>(e.evaluate(a[6+page_offset])));
	int mode = static_cast<int>(std::get<Number>(e.evaluate(a[7+page_offset])));
	
	std::array<unsigned char, WIDTH*HEIGHT> temp;
	for (int x = source_x1; x <= source_x2; ++x){
		for (int y = source_y1; y <= source_y2; ++y){
			temp[x+WIDTH*y] = image[page].at(4*(x+WIDTH*y));
		}
	}
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	for (int x = source_x1; x <= source_x2; ++x){
		for (int y = source_y1; y <= source_y2; ++y){
			int dx = x - source_x1 + dest_x;
			int dy = y - source_y1 + dest_y;
			if (mode || temp[x+WIDTH*y])
				draw_pixel(image[drawpage[screen]], g, dx, dy, temp[x+WIDTH*y]);
		}
	}
}

/// PTC function to get the color at a point.
/// @warning Does not implement the 3-argument version yet.
/// 
/// Format: 
/// * `GSPOIT(x,y)`
/// 
/// Values:
/// * x: x coordinate
/// * y: y coordinate
/// 
/// @param v Values
/// @return Pixel color at (x,y)
Var Graphics::gspoit_(const Vals& v){
	int x = std::get<Number>(v.at(0));
	int y = std::get<Number>(v.at(1));
	
//	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	auto r = Var(static_cast<Number>(image[drawpage[screen]].at(4*(x+256*y))));
	return r;
}

void Graphics::reset(){
	//GDRAWMD FALSE:GCLS (all):GCOLOR 0'(all GRP)
	gdrawmd = false;
	gcolor = 0;
	// Following clears all GRP
	for (int i = 0; i < 4; ++i){
		auto& g = grp.at("GRP"+std::to_string(i)).data;
		for (int y = 0; y < HEIGHT; ++y){
			for (int x = 0; x < WIDTH; ++x){
				draw_pixel(image[i], g, x, y, gcolor);
			}
		}
	}
}

void Graphics::regen_grp(int page){
	for (int x = 0; x < 256; ++x){
		for (int y = 0; y < 192; ++y){
			image[page][4*(x+256*y)] = grp.at("GRP"+std::to_string(page)).data[to_chr_coords(x,y)];
		}
	}
}

int Graphics::get_prio(int screen){
	return prio[screen];
}

std::array<unsigned char, Graphics::WIDTH*Graphics::HEIGHT*4>& Graphics::draw(int screen){
	return image[displaypage[screen]];
}
