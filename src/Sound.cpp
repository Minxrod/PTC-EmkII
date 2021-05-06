#include "Sound.h"

Sound::Sound(Evaluator& ev) : e{ev}{
}

void Sound::beep_(const Args&){
	//do nothing because I haven't figured out how to handle sound yet
}

void Sound::bgmplay_(const Args&){
	//do nothing because I haven't figured out how to handle sound yet
}

Var Sound::bgmchk_(const Vals&){
	//return whether sound is playing (always false because sound is not implemented yet)
	return Var(0.0);
}


std::map<Token, cmd_type> Sound::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map{"BEEP"_TC, getfunc(this, &Sound::beep_)},
		cmd_map{"BGMPLAY"_TC, getfunc(this, &Sound::bgmplay_)},
//		cmd_map{"BGMSTOP"_TC, getfunc(this, &Sound::bgmstop_)},
	};
}

std::map<Token, op_func> Sound::get_funcs(){
	return std::map<Token, op_func>{
		func_map{"BGMCHK"_TF, getfunc(this, &Sound::bgmchk_)},
	};
}
