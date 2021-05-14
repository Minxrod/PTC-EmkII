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

void Graphics::gpset_(const Args& a){
	//GPSET X,Y[,C]
	auto col = a.size() == 4 ? gcolor : static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[3])));
	
	auto& g = grp.at("GRP"+std::to_string(drawpage[screen])).data;
	
	g.at(to_chr_coords(x, y)) = col;
}

void Graphics::gline_(const Args&){

}

void Graphics::gcirc_(const Args&){

}

void Graphics::gpaint_(const Args&){

}

void Graphics::gbox_(const Args&){

}

void Graphics::gfill_(const Args&){

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
			image[(x+WIDTH*y)*4] = vec[x+WIDTH*y];
		}
	}
	
	return image;
}
