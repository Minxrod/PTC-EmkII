#pragma once

#include <SFML/Audio.hpp>
#include <SSEQStream.h>

#include <map>
#include <vector>
#include <memory>

#include "Vars.h"
#include "Evaluator.h"

class Sound {
	const static int BGM_TRACK_COUNT = 8;
	const static int SFX_COUNT = 16;
	const static int SEQ_SFX_OFFSET = 30;
	
	
	Evaluator& e;
	
	const std::string resource_path{"resources/sounds/"};

//	std::vector<sf::SoundBuffer> wav;
//	std::vector<sf::Sound> sfx;
//	std::vector<sf::Music> bgm;
	
	SWAR swar;
	SBNK sbnk;
	std::vector<SSEQ> sseq;
	
	std::vector<std::unique_ptr<SSEQStream>> bgm;
	std::vector<std::unique_ptr<SSEQStream>> sfx;
	
	int get_available_sound();
	
	void beep_(const Args&);

	void bgmplay_(const Args&);
	void bgmstop_(const Args&);
	void bgmvol_(const Args&);

	Var bgmchk_(const Vals&);

public:
	Sound(Evaluator&);

	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
};
