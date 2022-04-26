#include "Console.h"
#include <algorithm>

Console::Console(Evaluator& eval, CHR& chr, Input& i) : 
in{i}, e{eval}, c{chr}, tm{WIDTH,HEIGHT}{
	cur_fg_color = 0;
	cur_bg_color = 0;
	cur_x = std::get<Number*>(e.vars.get_var_ptr("CSRX"));
	cur_y = std::get<Number*>(e.vars.get_var_ptr("CSRY"));
	tabstep = std::get<Number*>(e.vars.get_var_ptr("TABSTEP"));
	*tabstep = 4;
}

std::map<Token, cmd_type> Console::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("CLS"_TC, getfunc<Console>(this, &Console::cls_)),
		cmd_map("PRINT"_TC, getfunc<Console>(this, &Console::print_)),
		cmd_map("COLOR"_TC, getfunc<Console>(this, &Console::color_)),
		cmd_map("LOCATE"_TC, getfunc<Console>(this, &Console::locate_)),
		cmd_map("INPUT"_TC, getfunc<Console>(this, &Console::input_)),
		cmd_map("LINPUT"_TC, getfunc<Console>(this, &Console::linput_)),
		cmd_map("OK"_TC, getfunc<Console>(this, &Console::ok)),
	};
}

std::map<Token, op_func> Console::get_funcs(){
	return std::map<Token, op_func>{
		func_map("CHKCHR"_TF, getfunc<Console>(this, &Console::chkchr_)),
	};
}


void Console::cls_(const Args&){
	*cur_x = 0;
	*cur_y = 0;
	std::fill(text.begin(), text.end(), 0);
	std::fill(fg_color.begin(), fg_color.end(), cur_fg_color);
	std::fill(bg_color.begin(), bg_color.end(), cur_bg_color);
}

void Console::reset(){
	cur_fg_color = 0;
	cur_bg_color = 0;
	cls_({});
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
			if ((*cur_x == 31 && !sub_exp.empty()) || (i == (int)a.size()-1 && !sub_exp.empty()))
				newline();
		}
		if (i != (int)a.size()-1){
			tab();
		}
	}
}

std::string printable(const Var& v){
	std::string str;
	if (std::holds_alternative<Number>(v)){
		str = std::to_string(std::get<Number>(v));
		//some assumptions: to_string outputs a number of form [aaaaa]a.bbbbbb
		//currently, the type Number = double, so this works reliably.
		str = str.substr(0, str.find(".")+4);
		while (str.back() == '0'){
			str = str.substr(0, str.size()-1);
		}
		if (str.back() == '.')
			str = str.substr(0, str.size()-1);		
		
	} else {
		str = std::get<String>(v);
	}
	return str;	
}

void Console::print_str(std::string str){
//	if (str.find("tod") != std::string::npos)
//		throw std::runtime_error{"found tod"};
	for (int i = 0; i < (int)str.size(); ++i){
		char c = str[i];
		text[*cur_x+WIDTH * *cur_y] = c;
		bg_color[*cur_x + WIDTH * *cur_y] = cur_bg_color;
		fg_color[*cur_x + WIDTH * *cur_y] = cur_fg_color;
		
		if(i != (int)str.size()-1 || (*cur_x != 31 || *cur_y != 23))
			advance();
	}
}

void Console::print_(const Var& v){
	std::string str = printable(v);
	print_str(str);
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
	
	auto old_x = *cur_x;
	auto old_y = *cur_y;
	
	//do inputs loop
	char lastpress = '\0';
	std::string res = "";
	while (lastpress != '\r'){
		lastpress = in.inkey_internal();
		
		if (lastpress == '\b'){
			res = res.substr(0, res.size()-1);
			--*cur_x;
			print_str(" ");
		} else if (lastpress != '\0' && lastpress != '\r' && lastpress != '\b' && res.size() < WIDTH) {
			res += lastpress;
		}
		
		*cur_x = old_x;
		*cur_y = old_y;
		print_str(res);
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
	*cur_x = (int)std::get<Number>(e.evaluate(a[1]));
	*cur_y = (int)std::get<Number>(e.evaluate(a[2]));
}

void Console::newline(){
	*cur_x = 0;
	(*cur_y)++;
	if (*cur_y >= HEIGHT){
		scroll();
	}
}

void Console::scroll(){
	*cur_x = 0;
	*cur_y = HEIGHT-1;
	std::copy(text.begin() + WIDTH, text.end(), text.begin());
	std::copy(fg_color.begin() + WIDTH, fg_color.end(), fg_color.begin());
	std::copy(bg_color.begin() + WIDTH, bg_color.end(), bg_color.begin());
	std::fill(text.begin() + WIDTH * 23, text.end(), 0);
}

//returns true if console needed to scroll
bool Console::advance(){
	++*cur_x;
	if (*cur_x >= WIDTH){
		newline();
		return true;
	}
	return false;
}

void Console::tab(){
	do
	{
		text[*cur_x + WIDTH * *cur_y] = 0;
		bg_color[*cur_x + WIDTH * *cur_y] = cur_bg_color;
		fg_color[*cur_x + WIDTH * *cur_y] = cur_fg_color;
	} while (!advance() && static_cast<int>(*cur_x) % static_cast<int>(*tabstep) != 0);
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
	if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT){
		return Var(-1.0);
	}
	
	return Var(Number((unsigned char)text[x+WIDTH*y]));
}

void Console::print(int x, int y, Var& v, int c){
	cur_fg_color = c;
	*cur_x = x;
	*cur_y = y;
	
	auto str = std::get<String>(v);
	if (*cur_x >= WIDTH)
		return;

	for (char c : str){		
		text[*cur_x+WIDTH * *cur_y] = c;
		bg_color[*cur_x + WIDTH * *cur_y] = cur_bg_color;
		fg_color[*cur_x + WIDTH * *cur_y] = cur_fg_color;

		if (advance())
			break;
	}
}

void Console::ok(const Args&){
	//maybe use args to pass error messages later on
	print_str("OK");
	newline();
}

TileMap& Console::draw(){
	for (int x = 0; x < WIDTH; ++x){
		for (int y = 0; y < HEIGHT; ++y){
			tm.tile(x,y,text[x+WIDTH*y]);
			tm.palette(x,y,16*fg_color[x+WIDTH*y],bg_color[x+WIDTH*y]>0 ? 15+16*bg_color[x+WIDTH*y] : 0);
		}
	}
	return tm;
}
