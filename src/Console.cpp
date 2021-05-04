#include "Console.h"
#include <algorithm>

Console::Console(Evaluator& eval, CHR& chr, Input& i) : in{i}, e{eval}, c{chr}{
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
		cmd_map("INPUT"_TC, getfunc<Console>(this, &Console::input_)),
		cmd_map("LINPUT"_TC, getfunc<Console>(this, &Console::linput_)),
	};
}

std::map<Token, op_func> Console::get_funcs(){
	return std::map<Token, op_func>{
		func_map("CHKCHR$"_TF, getfunc<Console>(this, &Console::chkchr_)),
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
		//some assumptions: to_string outputs a number of form [aaaaa]a.bbbbbb
		//currently, the type Number = double, so this works reliably.
		str = str.substr(0, str.find(".")+3);
		while (str.back() == '0'){
			str = str.substr(0, str.size()-1);
		}
		if (str.back() == '.')
			str = str.substr(0, str.size()-1);		
		
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

std::pair<std::vector<Token>, std::string> Console::input_common(const Args& a){
	auto& guide = a[1];
	auto str = std::vector<Token>(guide.begin(), std::find(guide.begin(), guide.end(), ";"_TO));
	auto var = std::vector<Token>(std::find(guide.begin(), guide.end(), ";"_TO)+1, guide.end());
	if (var.empty())
		var = str; //no semicolon means a[1] only contains <varname>
	
	print_(e.evaluate(str));
	print_(Var(String("?")));
	newline();
	
	auto old_x = cur_x;
	auto old_y = cur_y;
	
	//do inputs loop
	char lastpress = '\0';
	std::string res = "";
	while (lastpress != '\r'){
		lastpress = in.inkey_internal();
		
		if (lastpress == '\b'){
			res = res.substr(0, res.size()-1);
		} else if (lastpress != '\0' && lastpress != '\r' && lastpress != '\b' && res.size() < WIDTH) {
			res += lastpress;
		}
		
		cur_x = old_x;
		cur_y = old_y;
		print_(Var(res));
	}
	newline();
	
	return std::pair(var, res);
}

void Console::input_(const Args& a){
	//INPUT [string;]a$,b,c
	auto [var, str] = input_common(a);
	
	std::cout << "Input: " << str << std::endl;
	
	std::vector<Token> results{Token{"", Type::Str}};
	for (auto c : str){
		if (c == ',')
			results.push_back(Token{"", Type::Str});
		else
			results.back().text += c; 
	}
	//args& a = {INPUT, string;var, var, var};
	//results = {STR, STR, STR}
	for (auto i = 1; i < (int)a.size(); ++i){
		auto& name_exp = (i==1 ? var : a[i]);
		
		e.assign(name_exp, results[i-1]);
	}
}

void Console::linput_(const Args& a){
	//LINPUT [string;]a$
	auto [var, str] = input_common(a);
	
	e.assign(var, Token{str, Type::Str});
}

void Console::locate_(const Args& a){
	//LOCATE <x> <y>
	cur_x = (int)std::get<Number>(e.evaluate(a[1]));
	cur_y = (int)std::get<Number>(e.evaluate(a[2]));
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
	std::copy(fg_color.begin() + WIDTH, fg_color.end(), fg_color.begin());
	std::copy(bg_color.begin() + WIDTH, bg_color.end(), bg_color.begin());
}

//returns true if console needed to scroll
bool Console::advance(){
	++cur_x;
	if (cur_x >= WIDTH){
		newline();
		return true;
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


void Console::color_(const Args& a){
	
	cur_fg_color = (int)std::get<Number>(e.evaluate(a[1]));
	if (a.size() == 3){
		cur_bg_color = (int)std::get<Number>(e.evaluate(a[2]));
	}
}

Var Console::chkchr_(const Vals& v){
	auto x = (int)std::get<Number>(v.at(0));
	auto y = (int)std::get<Number>(v.at(1));

	return Var(String(""+(char)text[x+WIDTH*y]));
}

std::array<unsigned char, Console::WIDTH*Console::HEIGHT*4*8*8>& Console::draw(){
	std::fill(image.begin(), image.end(), 0);
	for (int x = 0; x < 256; ++x){
		for (int y = 0; y < 192; ++y){
			int cx = x % 8;
			int cy = y % 8;
			int tx = x / 8;
			int ty = y / 8;
			auto col = c.get_pixel(text[tx+WIDTH*ty],cx,cy);
			image[(x+256*y)*4] = (!col) ? bg_color[tx+WIDTH*ty] : col + 16*fg_color[tx+WIDTH*ty];
		}
	}
	
	return image;
}
