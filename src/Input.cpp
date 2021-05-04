#include "Input.h"

Input::Input(Evaluator& ev) : e{ev}, button_info{12, std::vector<int>{0,0,0}}{
	
}

void Input::update(int b, Key k){
	buttons = b;
	keycode = code_to_ptc.at(k);
	//if keycode != (enter) and is in (valid ranges) add to inkey queue
	std::lock_guard loc(inkeybuf_mutex);
	inkeybuffer.push((char)code_to_ptc.at(k));
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
}
	
void Input::brepeat_(const Args& a){
	Number id = std::get<Number>(e.evaluate(a[1]));
	Number start = 0;
	Number repeat = 0;
	if (a.size() == 4){
		start = std::get<Number>(e.evaluate(a[2]));
		repeat = std::get<Number>(e.evaluate(a[3]));
	}
	
	std::lock_guard loc(button_mutex);
	button_info[id][1] = start;
	button_info[id][2] = repeat;	
}

Var Input::inkey_(const Vals&){
	std::lock_guard loc(inkeybuf_mutex);
	char c = '\0';
	if (!inkeybuffer.empty()){
		c = inkeybuffer.front();
		inkeybuffer.pop();
	}
	return String(""+c);
}

Var Input::button_(const Vals&){
	return Var(Number(buttons));
}

Var Input::btrig_(const Vals&){
	return Var(Number(buttons));	
}
	
std::map<Token, cmd_type> Input::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map{"BREPEAT"_TC, getfunc<Input>(this, &Input::brepeat_)},
	};
}

std::map<Token, op_func> Input::get_funcs(){
	return std::map<Token, op_func>{
		func_map{"BUTTON"_TF, getfunc<Input>(this, &Input::button_)},
		func_map{"BTRIG"_TF, getfunc<Input>(this, &Input::btrig_)},
		func_map{"INKEY$"_TF, getfunc<Input>(this, &Input::inkey_)},
	};
}
