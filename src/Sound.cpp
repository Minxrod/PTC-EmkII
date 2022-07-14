#include "Sound.hpp"

#include "PTCSystem.hpp"

#include <cmath>

Sound::Sound(PTCSystem* s) : system{s}, e{*s->get_evaluator()}{
	try {
		swar.open("resources/sounds/SWAR_0.swar");
		sbnk.open("resources/sounds/SBNK_0.sbnk");
		
		sfx.resize(SFX_COUNT);
		bgm.resize(BGM_TRACK_COUNT);
		for (auto& s : sfx){
			s = std::make_unique<SSEQStream>(swar, sbnk);
		}
		
		for (auto& b : bgm){
			b = std::make_unique<SSEQStream>(swar, sbnk);
		}
		
		for (auto i = 0; i < 294; ++i){
			sseq.emplace_back();
			sseq.back().open("resources/sounds/SSEQ_"+std::to_string(i)+".sseq");
		}
		for (auto i = 128; i <= 256; ++i){
			user_songs.emplace_back();
			user_songs.back().mml("");
		}
	} catch (...){
		std::cout << "Error loading sound: disabling sound system\n\n";
		enabled = false;
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
	if (!enabled)
		return;
	
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
	sfx.at(i)->set_sseq(&sseq[wf+SEQ_SFX_OFFSET]);
	auto& s = *sfx.at(i);
	s.setPitch(std::pow(2, p/4096.0));
	s.setVolume(v);
	s.play();
}

void Sound::bgmplay_(const Args& a){
	if (!enabled)
		return;
	
	auto arg1 = e.evaluate(a[1]); // there should always be at least one argument
	if (std::holds_alternative<Number>(arg1)){
		// BGMPLAY [track] <song> [volume]
		int track = 0;
		int song = -1;
		double volume = 100.0; // assumed default?
		
		if (a.size() == 2){ 
			// BGMPLAY <song>
			song = std::get<Number>(arg1);
		} else if (a.size() >= 3){ 
			// BGMPLAY <track> <song>
			track = std::get<Number>(arg1);
			song = std::get<Number>(e.evaluate(a[2]));
		} else if (a.size() == 4){
			// BGMPLAY <track> <song> <volume>
			volume = std::get<Number>(e.evaluate(a[3])) / 127.0 * 100.0;
		} else if (a.size() > 4){
			throw std::runtime_error{"BGMPLAY too many args"};
		}
		
		if (0 <= song && song < 30){
			bgm.at(track)->set_sseq(&sseq[song]);
		} else if (128 <= song && song <= 256){
			bgm.at(track)->set_sseq(&user_songs[song-128]);
		}
		
		auto& music = *bgm.at(track);
		music.setVolume(volume);
		music.play();
	} else { //assume string
		std::string mml{};
		//note: should only be up to 9 strings according to wiki?
		for (std::size_t i = 1; i < a.size(); ++i){
			mml += std::get<String>(e.evaluate(a[i]));
		}
		
		SSEQ mml_sseq{};
		mml_sseq.mml(mml);
		user_songs[128] = mml_sseq;
		// will always play at full volume on track 0
		bgm.at(0)->set_sseq(&user_songs[128]);
		
		auto& music = *bgm.at(0);
		music.setVolume(100);
		music.play();
	}
}

void Sound::bgmstop_(const Args& a){
	if (!enabled)
		return;
	
	int track = 0;
	if (a.size() == 2){
		//BGMSTOP <track>
		track = (int)std::get<Number>(e.evaluate(a[1]));
	} 
	if (bgm.at(track))
		bgm.at(track)->stop();
	//if (a.size() == 3){
	//BGMSTOP <track> <fadetime>
	//TODO: implement this correctly
	//}
}

void Sound::bgmclear_(const Args& a){
	//BGMCLEAR [slot]
	SSEQ empty{};
	empty.mml("");
	if (a.size() == 2){
		int slot = (int)std::get<Number>(e.evaluate(a[1]));
		
		user_songs.at(slot-128) = empty;
	} else {
		for (int i = 128; i <= 256; ++i){
			user_songs.at(i-128) = empty;
		}
	}
}

void Sound::bgmset_(const Args& a){
	//BGMSET song,mml1[,mml2...]
	int slot = (int)std::get<Number>(e.evaluate(a[1]));
	std::string mml{};
	for (std::size_t i = 2; i < a.size(); ++i){
		mml += std::get<String>(e.evaluate(a[i]));
	}
	SSEQ mml_seq{};
	mml_seq.mml(mml);
	user_songs.at(slot-128) = mml_seq;
}

void Sound::bgmsetd_(const Args& a){
	//BGMSETD song,"@label"
	int slot = (int)std::get<Number>(e.evaluate(a[1]));
	std::string label = std::get<String>(e.evaluate(a[2]));
	
	system->get_program()->data_seek(label);
	Token data{"", Type::Str}; // add nothing on first iteration of loop
	std::string mml;
	while (data.text != "0"){
		mml += data.text;
		data = system->get_program()->read_expr(); //read zero, do not add
		std::cout << data.text << std::endl;
	}
	
	SSEQ mml_seq{};
	mml_seq.mml(mml);
	user_songs.at(slot-128) = mml_seq;
}

void Sound::bgmprg_(const Args& a){
	//BGMPRG instr, waveform
	//BGMPRG instr, key, waveform
	//BGMPRG instr, a, d, s, r, waveform
	//BGMPRG instr, key, a, d, s, r, waveform
	
	int instr = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	auto& instrument = sbnk.instruments[instr];
	auto& note_def = instrument.get_note_def(0);
	int attack = note_def.attack;
	int decay = note_def.decay;
	int sustain = note_def.sustain;
	int release = note_def.release;
	int key = 60;
	std::string waveform;
	if (a.size() == 3){
		waveform = std::get<String>(e.evaluate(a[2]));
	} else if (a.size() == 4){
		key = std::get<Number>(e.evaluate(a[2]));
		waveform = std::get<String>(e.evaluate(a[3]));
	} else if (a.size() == 7){
		attack = std::get<Number>(e.evaluate(a[2]));
		decay = std::get<Number>(e.evaluate(a[3]));
		sustain = std::get<Number>(e.evaluate(a[4]));
		release = std::get<Number>(e.evaluate(a[5]));
		waveform = std::get<String>(e.evaluate(a[6]));
	} else if (a.size() == 8){
		key = std::get<Number>(e.evaluate(a[2]));
		attack = std::get<Number>(e.evaluate(a[3]));
		decay = std::get<Number>(e.evaluate(a[4]));
		sustain = std::get<Number>(e.evaluate(a[5]));
		release = std::get<Number>(e.evaluate(a[6]));
		waveform = std::get<String>(e.evaluate(a[7]));
	}
	note_def.note = key;
	note_def.attack = attack;
	note_def.decay = decay;
	note_def.sustain = sustain;
	note_def.release = release;
	if (waveform.length() != 256 && waveform.length() != 128){
		throw std::runtime_error{"Invalid waveform size"};
	}
	auto& samples = swar.swav[note_def.swav_no].samples;
//	std::cout << "swav:" << note_def.swav_no;
//	std::cout << "samps:" << samples.size();

	for (std::size_t i = 0; i < samples.size()/2; ++i){
		char hi = waveform[(2*i) % waveform.length()];
		char lo = waveform[(2*i+1) % waveform.length()];
		
		int value = (hi > '9' ? hi - 'A' + 10 : hi - '0') << 4;
		value += (lo > '9' ? lo - 'A' + 10 : lo - '0');
		if (value >= 128)
			value -= 256;
		
		samples[2*i] = value * 256;
		samples[2*i+1] = value * 256;
	}
//	for (auto s : samples){
//		std::cout << (int)s << " ";
//	}
	
}

Var Sound::bgmchk_(const Vals& v){
	if (!enabled)
		return Var(0.0);
	
	int track = 0;
	if (v.size() == 1){
		track = (int)std::get<Number>(v.at(0));
	}
	
	if (!bgm.at(track))
		return Var(0.0); // no song loaded -> not playing
	// song is loaded: is it still playing?
	return Var(static_cast<double>(bgm.at(track)->getStatus() == SSEQStream::Playing));
}

void Sound::bgmvol_(const Args& a){
	//BGMVOL [track,]volume
	int track = 0;
	int volume;
	if (a.size() == 3){
		track = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
		volume = static_cast<int>(std::get<Number>(e.evaluate(a[2])));
	} else if (a.size() == 2){
		volume = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	} else {
		if (a.size() < 2)
			throw std::runtime_error{"Missing operand (BGMVOL)"};
		else
			throw std::runtime_error{"Syntax error (BGMVOL)"};
	}
	bgm.at(track)->setVolume(volume * 100.0 / 127.0);
}

std::map<Token, cmd_type> Sound::get_cmds(){
	return std::map<Token, cmd_type>{
		cmd_map{"BEEP"_TC, getfunc(this, &Sound::beep_)},
		cmd_map{"BGMPLAY"_TC, getfunc(this, &Sound::bgmplay_)},
		cmd_map{"BGMSTOP"_TC, getfunc(this, &Sound::bgmstop_)},
		cmd_map{"BGMCLEAR"_TC, getfunc(this, &Sound::bgmclear_)},
		cmd_map{"BGMSET"_TC, getfunc(this, &Sound::bgmset_)},
		cmd_map{"BGMSETD"_TC, getfunc(this, &Sound::bgmsetd_)},
		cmd_map{"BGMPRG"_TC, getfunc(this, &Sound::bgmprg_)},
		cmd_map{"BGMVOL"_TC, getfunc(this, &Sound::bgmvol_)},
	};
}

std::map<Token, op_func> Sound::get_funcs(){
	return std::map<Token, op_func>{
		func_map{"BGMCHK"_TF, getfunc(this, &Sound::bgmchk_)},
	};
}
