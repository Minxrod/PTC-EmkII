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
	
	sfx.resize(SFX_COUNT);
	bgm.resize(BGM_TRACK_COUNT);
	
	swar.open("resources/sounds/SWAR_0.swar");
	sbnk.open("resources/sounds/SBNK_0.sbnk");
	
	for (auto i = 0; i < 294; ++i){
		sseq.emplace_back();
		sseq.back().open("resources/sounds/SSEQ_"+std::to_string(i)+".sseq");
	}
}

int Sound::get_available_sound(){
	int i = 0;
	for (auto& s : sfx){
		if (!s || s->getStatus() == sf::SoundSource::Status::Stopped)
			return i;
		++i;
	}
	return 15;
}

void Sound::beep_(const Args& a){
	auto i = get_available_sound();
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
//	auto pan = 0;
	if (a.size() >= 5){ //BEEP <waveform> <pitch> <volume> <panpot>
//		pan = pan + 1; //don't quite know how to handle this one yet
	}
	if (sfx[i])
		sfx[i]->stop();
	sfx[i] = std::make_unique<SSEQStream>(swar, sbnk, sseq[wf+SEQ_SFX_OFFSET]);
	auto& s = *sfx[i];
	s.setPitch(std::pow(2, p/4096.0));
	s.setVolume(v);
	s.play();
}

void Sound::bgmplay_(const Args& a){
	auto arg1 = e.evaluate(a[1]); // there should always be at least one argument
	if (std::holds_alternative<Number>(arg1)){
		// BGMPLAY [track] <song> [volume]
		int track = 0;
		int song;
		double volume = 100.0; // assumed default?
		
		if (a.size() == 2){ 
			// BGMPLAY <song>
			song = std::get<Number>(arg1);
		} else if (a.size() >= 3){ 
			// BGMPLAY <track> <song>
			track = std::get<Number>(arg1);
			song = std::get<Number>(e.evaluate(a[2]));
		}
		if (a.size() == 4){
			// BGMPLAY <track> <song> <volume>
			volume = std::get<Number>(e.evaluate(a[3])) / 127.0 * 100.0;
		} else if (a.size() > 4){
			throw std::runtime_error{"BGMPLAY too many args"};
		}
		
		if (bgm.at(track))
			bgm.at(track)->stop();
		bgm.at(track) = std::make_unique<SSEQStream>(swar, sbnk, sseq[song]);
		auto& music = *bgm.at(track);
		music.setVolume(volume);
		music.play();
	}
	//TODO: Implement MML, maybe?
}

void Sound::bgmstop_(const Args& a){
	int track = 0;
	if (a.size() == 2){
		//BGMSTOP <track>
		track = (int)std::get<Number>(e.evaluate(a[1]));
	} 
	bgm.at(track)->stop();
	//if (a.size() == 3){
	//BGMSTOP <track> <fadetime>
	//TODO: implement this correctly
	//}
}

Var Sound::bgmchk_(const Vals& v){
	int track = 0;
	if (v.size() == 1){
		track = (int)std::get<Number>(v.at(0));
	}
	
	if (!bgm.at(track))
		return Var(0.0); // no song loaded -> not playing
	// song is loaded: is it still playing?
	return Var(static_cast<double>(bgm.at(track)->getStatus() == sf::SoundSource::Status::Stopped));
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
