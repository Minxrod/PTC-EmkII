#include "Sound.h"

Sound::Sound(Evaluator& ev) : e{ev}{
	for (auto i = 0; i < 264; i++){
		sf::SoundBuffer buf;
		std::string name = "resources/sounds/sfx/beep"+std::to_string(i/100)+std::to_string((i/10)%10)+std::to_string(i%10)+".wav";
		if (buf.loadFromFile(name)){
			wav.push_back(buf);
		} else {
			throw std::runtime_error{"error loading file: " + name};
		}
	}
	
	sfx = std::vector<sf::Sound>(16, sf::Sound{});
}

sf::Sound& Sound::get_available_sound(){
	for (auto& s : sfx){
		if (s.getStatus() == sf::SoundSource::Status::Stopped)
			return s;
	}
	return sfx.at(15);
}

void Sound::beep_(const Args& a){
	auto& s = get_available_sound();
	if (a.size() == 1){ //BEEP
		s.setBuffer(wav.at(0));
	} else if (a.size() == 2){ //BEEP <waveform number>
		auto wf = (int)std::get<Number>(e.evaluate(a[1]));
		s.setBuffer(wav.at(wf));
	}
	s.play();
}

void Sound::bgmplay_(const Args&){
	//do nothing because I haven't figured out how to handle bgm yet
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
