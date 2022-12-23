#include "Logger.hpp"

#include "Input.hpp"

#include "Variables.hpp"

const std::map<int, std::string> unicode_to_ptc = {
	{1,"\x1"},{2,"\x2"},{3,"\x3"},{4,"\x4"},{5,"\x5"},{6,"\x6"},{7,"\x7"},{8,"\x8"},{9,"\x9"},{10,"\xa"},{11,"\xb"},{12,"\xc"},{13,"\xd"},{14,"\xe"},{15,"\xf"},
	{16,"\x10"},{17,"\x11"},{18,"\x12"},{19,"\x13"},{20,"\x14"},{21,"\x15"},{22,"\x16"},{23,"\x17"},{24,"\x18"},{25,"\x19"},{26,"\x1a"},{27,"\x1b"},{28,"\x1c"},{29,"\x1d"},{30,"\x1e"},{31,"\x1f"}, //control codes
	{32," "},{33,"!"},{34,"\""},{35,"#"},{36,"$"},{37,"%"},{38,"&"},{39,"'"},{40,"("},{41,")"},{42,"*"},{43,"+"},{44,","},{45,"-"},{46,"."},{47,"/"},
	{48,"0"},{49,"1"},{50,"2"},{51,"3"},{52,"4"},{53,"5"},{54,"6"},{55,"7"},{56,"8"},{57,"9"},{58,":"},{59,";"},{60,"<"},{61,"="},{62,">"},{63,"?"},
	{64,"@"},{65,"A"},{66,"B"},{67,"C"},{68,"D"},{69,"E"},{70,"F"},{71,"G"},{72,"H"},{73,"I"},{74,"J"},{75,"K"},{76,"L"},{77,"M"},{78,"N"},{79,"O"},
	{80,"P"},{81,"Q"},{82,"R"},{83,"S"},{84,"T"},{85,"U"},{86,"V"},{87,"W"},{88,"X"},{89,"Y"},{90,"Z"},{91,"["},{92,"\\"},{93,"]"},{94,"^"},{95,"_"},
	{96,"`"},{97,"a"},{98,"b"},{99,"c"},{100,"d"},{101,"e"},{102,"f"},{103,"g"},{104,"h"},{105,"i"},{106,"j"},{107,"k"},{108,"l"},{109,"m"},{110,"n"},{111,"o"},
	{112,"p"},{113,"q"},{114,"r"},{115,"s"},{116,"t"},{117,"u"},{118,"v"},{119,"w"},{120,"x"},{121,"y"},{122,"z"},{123,"{"},{124,"|"},{125,"}"},{126,"~"},{127,"\x7f"}, //end of ascii
	//(block symbols and pipes)
	//kana
	{12290,"\xa1"},{12296,"\xa2"},{12297,"\xa3"},{12289,"\xa4"},{12539,"\xa5"},{12530,"\xa6"},{12449,"\xa7"},{12451,"\xa8"},{12453,"\xa9"},{12455,"\xaa"},{12457,"\xab"},{12515,"\xac"},{12517,"\xad"},{12519,"\xae"},{12483,"\xaf"},
	{12540,"\xb0"},{12450,"\xb1"},{12452,"\xb2"},{12454,"\xb3"},{12456,"\xb4"},{12458,"\xb5"},{12459,"\xb6"},{12461,"\xb7"},{12463,"\xb8"},{12465,"\xb9"},{12467,"\xba"},{12469,"\xbb"},{12471,"\xbc"},{12473,"\xbd"},{12475,"\xbe"},{12477,"\xbf"},
	{12479,"\xc0"},{12481,"\xc1"},{12484,"\xc2"},{12486,"\xc3"},{12488,"\xc4"},{12490,"\xc5"},{12491,"\xc6"},{12492,"\xc7"},{12493,"\xc8"},{12494,"\xc9"},{12495,"\xca"},{12498,"\xcb"},{12501,"\xcc"},{12504,"\xcd"},{12507,"\xce"},{12510,"\xcf"},
	{12511,"\xd0"},{12512,"\xd1"},{12513,"\xd2"},{12514,"\xd3"},{12516,"\xd4"},{12518,"\xd5"},{12520,"\xd6"},{12521,"\xd7"},{12522,"\xd8"},{12523,"\xd9"},{12524,"\xda"},{12525,"\xdb"},{12527,"\xdc"},{12531,"\xdd"},{12443,"\xde"},{12444,"\xdf"},
	//(more symbols)
};


Input::Input(Evaluator& ev) : e{ev}, button_info{12, std::vector<int>{0,0,0}}{
	logger::log("Input", "Constructed Input object");
}

void Input::update(int b){
	old_buttons = (int)buttons;
	buttons = b;
	{
		std::lock_guard bloc(button_mutex);
		for (int i = 0; i < 12; ++i){
			if (buttons & (1 << i)){
				++button_info[i][0];
			} else {
				button_info[i][0] = 0;	
			}
		}
	}
}

void Input::type(int unicode, Key key){
	//check special keys first
	if (key == Key::Backspace){
		keycode = 15;
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(keycode));
		return;
	} else if (key == Key::Tab){
		keycode = 32;
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(keycode));
		for (int i = 0; i < (int)*std::get<Number*>(e.vars.get_var_ptr("TABSTEP")); ++i)
			inkeybuffer.push(' ');
		return;
	}
	
	std::string add{};
	if (unicode_to_ptc.count(unicode))
		add = unicode_to_ptc.at(unicode);
	else if (unicode < 0x100)
		add = unicode;
		
	type_try_keyboard(add, key);
}

void Input::type_try_keyboard(std::string keys, int){
	std::lock_guard loc(inkeybuf_mutex);
	if (keys.size()){
		for (auto c : keys){
			if (c)
				inkeybuffer.push(c);
			else
				break;
		}
		std::size_t key = kyx[keyboard].find(keys);
		if (key != std::string::npos)
			keycode = keyboard == 5 ? key/2 : key;
		else
			keycode = 0;
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(keycode));
	}
	if (shift & 1){
		shift &= 0xfe;
		keyboard ^= 1;
	}
}


void Input::touch_key(int key){
//	std::lock_guard loc(inkeybuf_mutex);
	std::string c = "\0";
	if (key){
		c = kyx[keyboard].at(keyboard == 5 ? 2*key : key);
		if (keyboard == 5){
			c += kyx[keyboard].at(2*key+1);
		}
	}
	keycode = key;
	e.vars.write_sysvar("KEYBOARD", static_cast<double>(key));
	if (key == 47){
		shift ^= 1;
		keyboard ^= 1;
	} else if (key == 61){
		shift ^= 2;
		keyboard ^= 1;
	} else if (key == 62){
		shift = 0;
		keyboard = 0;
	} else if (key == 63){
		shift = 0;
		keyboard = 2;
	} else if (key == 64){
		shift = 0;
		keyboard = 4;
	}
	
	if (c[0] != kyx[keyboard].at(0)){
		if (c[0] != '\t') {
			type_try_keyboard(std::string{c}, 0);
		} else {
			type_try_keyboard(std::string((int)*std::get<Number*>(e.vars.get_var_ptr("TABSTEP")), ' '), 0);
		}
	}
}

void Input::touch(bool t, int x, int y){
	if (x < 0 || x > 255 || y < 0 || y > 191)
		t = false; // do not allow out of bounds input
	if (t)
		++tchtime;
	else {
		tchtime=0;
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(0)); //can't touch keys if no touchscreen
	}
	tchst = t;
	if (t){
		tchx = x;
		tchy = y;
	}
	e.vars.write_sysvar("TCHX", static_cast<double>(tchx));
	e.vars.write_sysvar("TCHY", static_cast<double>(tchy));
	e.vars.write_sysvar("TCHTIME", static_cast<double>(tchtime));
	e.vars.write_sysvar("TCHST", tchst ? 1.0 : 0.0);
}

/// PTC command to set button repeat timings.
/// @note If start and repeat are omitted, repetition is disabled.
/// 
/// Format: 
/// * `BREPEAT button[,start,repeat]`
/// 
/// Arguments:
/// * button: Button id to set or clear repeat timings
/// * start: Number of frames before first repeat
/// * repeat: Number of frames before consecutive repeats
/// 
/// @param a Arguments
void Input::brepeat_(const Args& a){
	Number id = std::get<Number>(e.evaluate(a[1]));
	Number start = 0;
	Number repeat = 0;
	if (a.size() == 4){
		start = std::get<Number>(e.evaluate(a[2]));
		repeat = std::get<Number>(e.evaluate(a[3]));
	}
	
	//locks should not be necessary here
	std::lock_guard loc(button_mutex);
	button_info[id][1] = start;
	button_info[id][2] = repeat;
}

wchar_t Input::inkey_internal(){
	std::lock_guard loc(inkeybuf_mutex);
	char c = '\0';
	if (!inkeybuffer.empty()){
		c = inkeybuffer.front();
		inkeybuffer.pop();
	}
	return c;
}

int Input::keycode_internal(){
	int k = keycode;
	keycode = 0;
	return k;
}

/// PTC function to get a typed character.
/// 
/// Format: 
/// * `INKEY$()`
/// 
/// @param Values (ignored)
/// @return String containing character typed, or empty string if nothing was typed
Var Input::inkey_(const Vals&){
	String res = L"";
	auto c = inkey_internal();
	if (c)
		res += c;
	return Var(res);
}

/// PTC command to check the state of the buttons.
/// 
/// Format: 
/// * `BUTTON(mode)`
/// 
/// Values:
/// * mode: Button mode (default=0)
/// 
/// The button modes are: 
/// * 0=held
/// * 1=moment pressed, repeat on
/// * 2=moment pressed, repeat off
/// * 3=moment released
/// 
/// @param v Values
/// @return Button states in mode
Var Input::button_(const Vals& v){
	double b0 = static_cast<double>(buttons);
	double b2 = static_cast<double>(buttons & ~old_buttons);
	double b3 = static_cast<double>((buttons ^ old_buttons) & old_buttons);
	if (v.size() == 1){
		switch (static_cast<int>(std::get<Number>(v.at(0)))){
			case 0:
				return Var(b0);
			case 1:
				//moment pressed, repeat enabled
				return btrig_(v);
			case 2:
				//moment pressed no repeat
				return Var(b2);
			case 3:
				//moment released
				return Var(b3);
		}
		throw std::runtime_error{"Type doesn't exist BUTTON"};
	} else {
		return Var(b0);	
	}
}

/// PTC command to check the button press state, with repeats. Equivalent to BUTTON(1)
/// 
/// Format: 
/// * `BTRIG()`
/// 
/// @param Values (ignored)
/// @return Buttons pressed or repeating
Var Input::btrig_(const Vals&){
	int b = 0;
	//lock <maybe> not necessary? (only reads here, but button_info[x][0] may be modified elsewhere)
	std::lock_guard loc{button_mutex};
	for (int i = 0; i < 12; ++i){
		int time = button_info[i][0];
		int start = button_info[i][1];
		int repeat = button_info[i][2];

		b |= (time == 1)<<i;
		if (repeat > 0){ //repeat=0 -> disabled
			if (time > start){ //repeat only applies past start time
				b |= ((time - start) % (repeat+1) == 0)<<i;
			}
		}
	}
	logger::log("Input",b);
	return Var(Number(b));
}
	
std::map<Token, cmd_type> Input::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map{"BREPEAT"_TC, getfunc<Input>(this, &Input::brepeat_)},
	};
}

std::map<Token, op_func> Input::get_funcs(){
	return std::map<Token, op_func>{
		func_map{"BUTTON"_TF, getfunc(this, &Input::button_)},
		func_map{"BTRIG"_TF, getfunc(this, &Input::btrig_)},
		func_map{"INKEY$"_TF, getfunc(this, &Input::inkey_)},
	};
}
