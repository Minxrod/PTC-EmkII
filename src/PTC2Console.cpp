#include "PTC2Console.hpp"

#include "Visual.hpp"
#include "PTCSystem.hpp"

#include <regex>

/// Regex matching valid numeric INPUT.
const std::regex number{ R"(-?[0-9]+\.?[0-9]*)" };
/// Regex matching invalid INPUT for numeric variables.
///
/// These inputs are known to cause Overflow errors instead of the ?Redo from start prompt.
const std::regex bad_number{ R"([\-0-9]?\.[0-9]*\.)" };

PTC2Console::PTC2Console(PTCSystem* s) : BaseConsole(), system{s},
e{*system->get_evaluator()}, tm{PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT} {
	csrx = std::get<Number*>(e.vars.get_var_ptr("CSRX"));
	csry = std::get<Number*>(e.vars.get_var_ptr("CSRY"));
	tabstep = std::get<Number*>(e.vars.get_var_ptr("TABSTEP"));
	*csrx = get_x();
	*csry = get_y();
	*tabstep = get_tab();
}

std::map<Token, cmd_type> PTC2Console::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map("CLS"_TC, getfunc<PTC2Console>(this, &PTC2Console::cls_)),
		cmd_map("PRINT"_TC, getfunc<PTC2Console>(this, &PTC2Console::print_)),
		cmd_map("COLOR"_TC, getfunc<PTC2Console>(this, &PTC2Console::color_)),
		cmd_map("LOCATE"_TC, getfunc<PTC2Console>(this, &PTC2Console::locate_)),
		cmd_map("INPUT"_TC, getfunc<PTC2Console>(this, &PTC2Console::input_)),
		cmd_map("LINPUT"_TC, getfunc<PTC2Console>(this, &PTC2Console::linput_)),
		cmd_map("OK"_TC, getfunc<PTC2Console>(this, &PTC2Console::ok)),
	};
}

std::map<Token, op_func> PTC2Console::get_funcs(){
	return std::map<Token, op_func>{
		func_map("CHKCHR"_TF, getfunc<PTC2Console>(this, &PTC2Console::chkchr_)),
	};
}


/// PTC command to clear the console.
/// 
/// Format: 
/// * `CLS`
/// 
void PTC2Console::cls_(const Args&){
	BaseConsole::cls();
}

void PTC2Console::reset(){
	BaseConsole::color(0, 0);
	BaseConsole::cls();
}

/// PTC command to print to the text console.
/// 
/// Format: 
/// * `PRINT var[,var2...][;var2...]`
/// 
/// Arguments:
/// * var: Value to print (can be multiple, separated by commas, semicolons)
/// 
/// @param a Arguments
void PTC2Console::print_(const Args& a){
	//PRINT <exp> <exp2> <exp3> ...
	// If inTheStupidCorner, then the cursor needs to be advanced to the next line before printing.
	//std::cout << "P";
	//std::cout << get_x() << "," << get_y() << std::endl;

	if (inTheStupidCorner)
		newline();

	//std::cout << get_x() << "," << get_y() << std::endl;
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
				
				auto res = e.eval_all_results(sub_exp);
				if (res.size() > 1){
					
				}
				for (int i = res.size()-1; i >= 0; --i){
					print_(res.at(i));
				}
				old_end = sub_end+1;
			} while (sub_end != exp.end());
			//if in last column and not followed by semicolon || last expression not followed by semicolon
			bool has_semicolon = sub_exp.empty();
			inTheStupidCorner = has_semicolon && (get_x() == get_w() - 1) && (get_y() == get_h() - 1);
			
			if (!has_semicolon && (get_x() == get_w()-1 || i == (int)a.size()-1)){
//				if (!(*cur_x == 0 && i == (int)a.size()-1)) // previous print ended with newline, don't need another
				// just kidding, that only happens for the OK at the end of the program...
					newline();
			}
		}
		//at end of expression, needs to tab
		if (i != (int)a.size()-1){
			tab();
		}
	}
	
	//somehow this was never noticed sooner?
	if (a.size() == 1){
		newline();
	}
	//std::cout << get_x() << "," << get_y() << std::endl;
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

void PTC2Console::print_str(std::string str){
//	if (str.find("tod") != std::string::npos)
//		throw std::runtime_error{"found tod"};
// I have no idea what the above code was meant to do, but it's funny so it remains as a comment.
	BaseConsole::print(str);
	*csrx = get_x();
	*csry = get_y();
}

void PTC2Console::print_(const Var& v){
	std::string str = printable(v);
	print_str(str);
}

std::pair<std::vector<Token>, std::string> PTC2Console::input_common(const Args& a){
	Input& in = *system->get_input();
	
	auto pnltype = system->get_visual()->p.panel_on();
	if (pnltype <= 1){
		system->get_visual()->p.panel_override();
	}
	
	auto& guide = a[1];
	auto str = std::vector<Token>(guide.begin(), std::find(guide.begin(), guide.end(), ";"_TO));
	auto semicolon = std::find(guide.begin(), guide.end(), ";"_TO);
	auto var = std::vector<Token>(semicolon+(semicolon != guide.end()), guide.end());
	if (var.empty()){
		var = str; //no semicolon means a[1] only contains <varname>
	}
	
	bool valid = false;
	std::string res;
	while (!valid){
		if (semicolon != guide.end())
			print_(e.evaluate(str));
		print_(Var(String("?")));
		newline();
		
		auto old_x = get_x();
		auto old_y = get_y();
		
		//do inputs loop
		char lastpress = '\0';
		// INPUT uses currently existing console text as input...
		for (int x = 0; x < PTC2_CONSOLE_WIDTH; ++x){
			auto c = chkchr(x,old_y);
			if (c)
				res += c;
		}
		// TODO: cursor should start at x=zero, even if text is pre-filled
		// TODO: cursor should be movable with left/right, and A/Y button should work as enter/backspace, L/R shift
		// TODO: shift+backspace clears line
		while (lastpress != '\r'){
			lastpress = in.inkey_internal();
			auto keycode = in.keycode_internal();
			
			if (keycode == 15){
				res = res.substr(0, res.size()-1);
			} else if (lastpress != '\0' && lastpress != '\r' && res.size() < PTC2_CONSOLE_WIDTH) {
				res += lastpress;
			}
			
			locate(old_x, old_y);
			print_str(res + (keycode == 15 ? " " : ""));
		}
		newline();
		
		valid = true;
		std::size_t commas = 0;
		for (auto c : res)
			commas += c == ',';
		//check number of vars passed
		if (commas != a.size()-2)
			valid = false;
		// validate types
		auto itr = res.begin();
		for (auto i = 1; i < (int)a.size(); ++i){
			auto& name_exp = (i==1 ? var : a[i]);
			// string,3424,3423.432,,
			auto sub_res = std::string(itr, std::find(itr, res.end(), ','));
			itr = std::find(itr+1, res.end(), ',');
			if (itr != res.end()) //next character past comma
				++itr;
			
			if (std::holds_alternative<Number>(e.evaluate(name_exp))){
				if (!std::regex_match(sub_res, number)){
					valid = false;
					if (std::regex_match(sub_res, bad_number)){
						throw std::logic_error{"Overflow (INPUT)"};
					}
				}
			}
		}
		
		if (!valid){
			print_(Var(String("?Redo from start")));
			newline();
			res = "";
		}
	}
	
	system->get_visual()->p.panel_override(pnltype);
	return std::pair(var, res);
}

/// PTC command to get text or numeric input from the user.
/// 
/// Format: 
/// * `INPUT [prompt;]var[,var2...]`
/// 
/// Arguments:
/// * prompt: Optional guiding prompt
/// * var: Variable to store result to
/// 
/// @param a Arguments
void PTC2Console::input_(const Args& a){
	//INPUT [string;]a$,b,c
	auto [var, str] = input_common(a);
	
//	std::cout << "Input: " << str << std::endl;
	
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

/// PTC command to get text input from the user, but with commas.
/// 
/// Format: 
/// * `LINPUT [prompt;]var`
/// 
/// Arguments:
/// * prompt: Guiding text string
/// * var: String variable to store result to
/// 
/// @param a Arguments
void PTC2Console::linput_(const Args& a){
	//LINPUT [string;]a$
	auto [var, str] = input_common(a);
	
	e.assign(var, Token{str, Type::Str});
}

/// PTC command to set the cursor location.
/// 
/// Format: 
/// * `LOCATE x,y`
/// 
/// Arguments:
/// * x: X coordinate
/// * y: Y coordinate
/// 
/// @param a Arguments
void PTC2Console::locate_(const Args& a){
	//LOCATE <x> <y>
	//std::cout << "L";
	//std::cout << get_x() << "," << get_y() << std::endl;
	int x = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int y = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	
	locate(x, y);
	*csrx = x;
	*csry = y;
	inTheStupidCorner = x == get_w()-1 && y == get_h()-1;
	//std::cout << get_x() << "," << get_y() << std::endl;
}

void PTC2Console::tab(){
	set_tab(static_cast<int>(*tabstep));
	BaseConsole::tab();
	*csrx = get_x();
	*csry = get_y();
}

bool PTC2Console::advance(){
	bool res = BaseConsole::advance();
	*csrx = get_x();
	*csry = get_y();
	return res;
}

void PTC2Console::newline(){
	BaseConsole::newline();
	*csrx = get_x();
	*csry = get_y();
}

void PTC2Console::scroll(){
	BaseConsole::scroll();
	*csrx = get_x();
	*csry = get_y();
}

/// PTC command to set the text and background colors.
/// 
/// Format: 
/// * `COLOR fg[,bg]`
/// 
/// Arguments:
/// * fg: New foreground color
/// * bg: Background color (default=no change)
/// 
/// @param a Arguments
void PTC2Console::color_(const Args& a){
	//std::cout << "C";
	//std::cout << get_x() << "," << get_y() << std::endl;
	int fg = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int bg = a.size() == 3 ? static_cast<int>(std::get<Number>(e.evaluate(a[2]))) : get_bg();
	color(fg, bg);
	//std::cout << get_x() << "," << get_y() << std::endl;
}

/// PTC function to check a character on the console.
/// 
/// Format: 
/// * `CHKCHR(x,y)`
/// 
/// Arguments:
/// * x: X coordinate to check
/// * y: Y coordinate to check
/// 
/// @param v Values
/// @return Character code (-1 if (x,y) is out of bounds)
Var PTC2Console::chkchr_(const Vals& v){
	auto x = static_cast<int>(std::get<Number>(v.at(0)));
	auto y = static_cast<int>(std::get<Number>(v.at(1)));
	if (x < 0 || x > get_w() || y < 0 || y > get_h()){
		return Var(-1.0);
	}
	
	return Var(Number(static_cast<unsigned char>(chkchr(x, y))));
}

void PTC2Console::print(int x, int y, Var& v, int c){
	color(c);
	locate(x, y);
	
	auto str = std::get<String>(v);
	if (get_x() >= get_w())
		return;
	
	BaseConsole::print(str);
}

void PTC2Console::ok(const Args&){
	//maybe use args to pass error messages later on
	using namespace std::literals::string_literals;
	
	BaseConsole::print("OK"s);
	newline();
}

TileMap& PTC2Console::draw(){
	for (int x = 0; x < get_w(); ++x){
		for (int y = 0; y < get_h(); ++y){
			tm.tile(x, y, static_cast<unsigned char>(chkchr(x,y)));
			tm.palette(x, y, 16 * get_fg(x,y), get_bg(x,y) > 0 ? 15 + 16 * get_bg(x,y) : 0);
		}
	}
	return tm;
}
