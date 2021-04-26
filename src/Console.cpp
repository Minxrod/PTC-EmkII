#include "Console.h"
#include <algorithm>

Console::Console(Evaluator& eval, CHR& chr) : e{eval}, c{chr}{
	cur_fg_color = 0;
	cur_bg_color = 0;
	cur_x = 0;
	cur_y = 0;
	tabstep = 4;
}

std::map<Token, cmd_type> Console::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("CLS"_TC, getfunc<Console>(this, &Console::cls_)),
		cmd_map("PRINT"_TC, getfunc<Console>(this, &Console::print_)),
		cmd_map("COLOR"_TC, getfunc<Console>(this, &Console::color_)),
		cmd_map("LOCATE"_TC, getfunc<Console>(this, &Console::locate_)),
	};
}

void Console::cls_([[maybe_unused]]const Args& a){
	std::fill(text.begin(), text.end(), 0);
	std::fill(fg_color.begin(), fg_color.end(), 0);
	std::fill(bg_color.begin(), bg_color.end(), 0);
}

void Console::print_(const Args& a){
	//PRINT <exp> <exp2> <exp3> ...
	
	for (int i = 1; i < (int)a.size(); ++i){
		auto& exp = a[i];
		if (!exp.empty()){
			auto old_end = exp.begin();
			auto sub_end = old_end;
			Expr sub_exp{};
			do {
				//<val1>;<val2>  ;
				sub_end = std::find(old_end, exp.end(), ";"_TO);
				sub_exp = Expr(old_end, sub_end);
				
				if (sub_exp.empty()){ //expression is only ; or at end of var;
					break;
				}
				
				auto res = e.evaluate(sub_exp);
				print_(res);
				old_end = sub_end+1;
			} while (sub_end != exp.end());
			//at end of expression, needs to tab
			if (i == (int)a.size()-1 && !sub_exp.empty())
				newline();
		}
		if (i != (int)a.size()-1){
			tab();
		}
	}
}

void Console::print_(const Var& v){
	std::string str;
	if (std::holds_alternative<Number>(v)){
		str = std::to_string(std::get<Number>(v));
	} else {
		str = std::get<String>(v);
	}
	
	for (char c : str){
		text[cur_x+WIDTH*cur_y] = c;
		bg_color[cur_x + WIDTH * cur_y] = cur_bg_color;
		fg_color[cur_x + WIDTH * cur_y] = cur_fg_color;

		advance();
	}

}

void Console::locate_(const Args& a){
	//LOCATE <x> <y>
	cur_x = (int)std::get<Number>(e.evaluate(a[1]));
	cur_y = (int)std::get<Number>(e.evaluate(a[1]));
}

void Console::newline(){
	cur_x = 0;
	cur_y++;
	if (cur_y >= HEIGHT){
		scroll();
	}
}

void Console::scroll(){
	cur_x = 0;
	cur_y = HEIGHT-1;
	std::copy(text.begin() + WIDTH, text.end(), text.begin());
}

//returns true if console needed to scroll
bool Console::advance(){
	++cur_x;
	if (cur_x >= WIDTH){
		++cur_y;
		if (cur_y >= HEIGHT){
			scroll();
			return true;
		}
	}
	return false;
}

void Console::tab(){
	do
	{
		text[cur_x + WIDTH * cur_y] = 0;
		bg_color[cur_x + WIDTH * cur_y] = cur_bg_color;
		fg_color[cur_x + WIDTH * cur_y] = cur_fg_color;
	} while (!advance() && cur_x % tabstep != 0);
}


void Console::color_(const Args& ){
	
}

Var Console::chkchr_(const Expr& ){
	return Var(0);
}

std::array<unsigned char, Console::WIDTH*Console::HEIGHT*4*8*8>& Console::draw(){
	std::fill(image.begin(), image.end(), 1);
	for (int x = 0; x < 256; ++x){
		for (int y = 0; y < 192; ++y){
			int cx = x % 8;
			int cy = y % 8;
			int tx = x / 8;
			int ty = y / 8;
			auto col = c.get_pixel(text[tx+WIDTH*ty],cx,cy);
			image[(x+256*y)*4] = col; //(!col) ? bg_color[tx+WIDTH*ty] : col + 16*fg_color[tx+WIDTH*ty];
		}
	}
	
	return image;
}
