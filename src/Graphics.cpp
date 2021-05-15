#include "Graphics.h"

Graphics::Graphics(Evaluator& ev, std::map<std::string, GRP>& grps) : 
	e{ev},
	grp{grps}
{}

std::map<Token, cmd_type> Graphics::get_cmds(){
	return std::map<Token, cmd_type>{
		std::pair<Token, cmd_type>("GPAGE"_TC, getfunc(this, &Graphics::gpage_)),
		std::pair<Token, cmd_type>("GCLS"_TC, getfunc(this, &Graphics::gcls_)),
		std::pair<Token, cmd_type>("GCOLOR"_TC, getfunc(this, &Graphics::gcolor_)),
		std::pair<Token, cmd_type>("GPSET"_TC, getfunc(this, &Graphics::gpset_)),
		std::pair<Token, cmd_type>("GLINE"_TC, getfunc(this, &Graphics::gline_)),
		std::pair<Token, cmd_type>("GBOX"_TC, getfunc(this, &Graphics::gbox_)),
		std::pair<Token, cmd_type>("GFILL"_TC, getfunc(this, &Graphics::gfill_)),

	};
}

std::map<Token, op_func> Graphics::get_funcs(){
	return std::map<Token, op_func>{};
}

void Graphics::gcolor_(const Args& a){
	//GCOLOR color
	gcolor = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
}

void Graphics::gcls_(const Args& a){
	//GCLS [color]
	auto col = a.size() == 1 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	std::fill(g.begin(), g.end(), col);
}

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

int to_chr_coords(int x, int y){
	int tx = x % 8;
	int ty = y % 8;
	int cx = (x / 8) % 8;
	int cy = (y / 8) % 8;
	int bx = x / 64;
	int by = y / 64;
	
	return tx + 8 * ty + cx * 64 + cy * 512 + bx * 4096 + by * 4096*4;
}

void draw_pixel(std::vector<unsigned char>& g, const int x, const int y, const int c){
	if (x > 0 && y > 0 && x < 256 && y < 192)
		g.at(to_chr_coords(x,y)) = c;
}

void Graphics::gpset_(const Args& a){
	//GPSET X,Y[,C]
	auto col = a.size() == 3 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	draw_pixel(g,x,y,col);
}

void draw_line(std::vector<unsigned char>& g, const int x1, const int y1, const int x2, const int y2, const int c){
	if (x2==x1){
		//vertical line
		int dir_y = y2>y1 ? 1 : -1;
		for (int y = y1; y != y2; y+=dir_y){
			draw_pixel(g,x1,y,c);
		}
	} else {
		int dir_x = x2>x1 ? 1 : -1;
		double m = static_cast<double>(y2-y1)/(static_cast<double>(x1-x2)*(dir_x));
		
		double y = y1;
		double ystep = 0;
		for(int x = x1; x != x2; x += dir_x){
			ystep += m;
			int ystepdir = ystep>0 ? -1 : 1;
			for (int yp = static_cast<int>(ystep); yp != 0; yp+=ystepdir){
				draw_pixel(g,x,y,c);
				y+=ystepdir;
				ystep+=ystepdir;
			}
		draw_pixel(g,x,y,c);
		}	
	}
}

void Graphics::gline_(const Args& a){
	//GLINE x1 y1 x2 y2 [c]
	auto col = a.size() == 5 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[5])));
	int x1 = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y1 = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int x2 = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	int y2 = static_cast<int>(std::get<Number>(e.evaluate(a[4])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;

	draw_line(g,x1,y1,x2,y2,col);
}

void Graphics::gcirc_(const Args&){
	//oh nooo I need to implement a circle alg??
}

void Graphics::gpaint_(const Args&){

}

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
				draw_pixel(g,x,y,col);
			}
		}
	}
}

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
			draw_pixel(g,x,y,col);
		}
	}
}

void Graphics::gputchr_(const Args&){

}

void Graphics::gdrawmd_(const Args&){

}

void Graphics::gprio_(const Args&){

}

void Graphics::gcopy_(const Args&){

}

std::array<unsigned char, Graphics::WIDTH*Graphics::HEIGHT*4>& Graphics::draw(){
	std::fill(image.begin(), image.end(), 0);
	auto& vec = grp.at("GRP"+std::to_string(drawpage[0])).data;
	
	for (int y = 0; y < HEIGHT; ++y){
		for (int x = 0; x < WIDTH; ++x){
			image[(x+WIDTH*y)*4] = vec[to_chr_coords(x,y)];
		}
	}
	
	return image;
}
