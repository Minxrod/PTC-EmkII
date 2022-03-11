#include "Input.h"

#include "Variables.h"

Input::Input(Evaluator& ev) : e{ev}, button_info{12, std::vector<int>{0,0,0}}{
	
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
	//if keycode != (enter) and is in (valid ranges) add to inkey queue
	/*{
		std::lock_guard loc(inkeybuf_mutex);
		auto c = '?';
		if (code_to_ptc.count(k) != 0){
			keycode = code_to_ptc.at(k);
			c = kya.at(keycode);
		}
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(keycode));
		if (c != kya.at(0))
			inkeybuffer.push(kya.at(code_to_ptc.at(k)));
	}*/
}

int Input::keyboard_to_keycode(Key k){
	if (code_to_ptc.count(k) != 0)
		return code_to_ptc.at(k);
	return 0; //invalid key...
}

void Input::touch(bool t, int x, int y){
	if (t)
		++tchtime;
	else
		tchtime=0;
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

void Input::touch_key(int keycode){
	{
		std::lock_guard loc(inkeybuf_mutex);
		auto c = '?';
		if (keycode != 0){
			c = kya.at(keycode);
		}
		e.vars.write_sysvar("KEYBOARD", static_cast<double>(keycode));
		if (c != kya.at(0))
			inkeybuffer.push(kya.at(keycode));
	}
}
	
void Input::brepeat_(const Args& a){
	Number id = std::get<Number>(e.evaluate(a[1]));
	Number start = 0;
	Number repeat = 0;
	if (a.size() == 4){
		start = std::get<Number>(e.evaluate(a[2]));
		repeat = std::get<Number>(e.evaluate(a[3]));
	}
	
	//locks should not be necessary here
	//std::lock_guard loc(button_mutex);
	button_info[id][1] = start;
	button_info[id][2] = repeat;
}

char Input::inkey_internal(){
	std::lock_guard loc(inkeybuf_mutex);
	char c = '\0';
	if (!inkeybuffer.empty()){
		c = inkeybuffer.front();
		inkeybuffer.pop();
	}
	return c;
}

Var Input::inkey_(const Vals&){
	std::string res = "";
	res += inkey_internal();
	return Var(res);
}

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
		return Var(b0);
	} else {
		return Var(b0);	
	}
}

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
			if (time >= start){ //repeat only applies past start time
				b |= ((time - start) % (repeat+1) == 0)<<i;
			}
		}
	}
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
