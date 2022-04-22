#include "Sound.h"

#include <cmath>

Sound::Sound(Evaluator& ev) : e{ev}{
/*	for (auto i = 0; i < 264; i++){
		sf::SoundBuffer buf;
		std::string name = "resources/sounds/sfx/beep"+std::to_string(i/100)+std::to_string((i/10)%10)+std::to_string(i%10)+".wav";
		if (buf.loadFromFile(name)){
			wav.push_back(buf);
		} else {
			throw std::runtime_error{"error loading file: " + name};
		}
	}*/
	
	sfx = std::vector<sf::Sound>(16, sf::Sound{});
}

sf::Sound& Sound::get_available_sound(){
	for (auto& s : sfx){
		if (s.getStatus() == sf::SoundSource::Status::Stopped)
			return s;
	}
	return sfx.at(15);
}

void Sound::beep_(const Args& ){
/*	auto& s = get_available_sound();
	int wf = 0;
	if (a.size() >= 2){ //BEEP <waveform number>
		wf = (int)std::get<Number>(e.evaluate(a[1]));
	}
	double p = 0;
	if (a.size() >= 3){ //BEEP <waveform number>,<pitch>
		p = std::get<Number>(e.evaluate(a[2]));
	}
	double v = 100;
	if (a.size() >= 4){ //BEEP <waveform> <pitch> <volume>
		v = std::get<Number>(e.evaluate(a[3]));
		v = v / 127.0 * 100.0;
	}
	auto pan = 0;
	if (a.size() >= 5){ //BEEP <waveform> <pitch> <volume> <panpot>
		pan = pan + 1; //don't quite know how to handle this one yet
	}
	s.setBuffer(wav.at(wf));
	s.setPitch(std::pow(2, p/4096.0));
	s.setVolume(v);
	s.play();*/
}

void Sound::bgmplay_(const Args&){
	//do nothing because I haven't figured out how to handle bgm yet
}

void Sound::bgmstop_(const Args&){
	//do nothing because all bgm is already stopped if it never starts
}

Var Sound::bgmchk_(const Vals&){
	//return whether sound is playing (always false because sound is not implemented yet)
	return Var(0.0);
}

std::map<Token, cmd_type> Sound::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map{"BEEP"_TC, getfunc(this, &Sound::beep_)},
		cmd_map{"BGMPLAY"_TC, getfunc(this, &Sound::bgmplay_)},
		cmd_map{"BGMSTOP"_TC, getfunc(this, &Sound::bgmstop_)},
	};
}

std::map<Token, op_func> Sound::get_funcs(){
	return std::map<Token, op_func>{
		func_map{"BGMCHK"_TF, getfunc(this, &Sound::bgmchk_)},
	};
}
