#pragma once

#include <SFML/Audio.hpp>

#include <map>
#include <vector>

#include "Vars.h"
#include "Evaluator.h"

class Sound {
	Evaluator& e;
	
	const std::string resource_path{"resources/sounds/"};

	std::vector<sf::SoundBuffer> wav;
	std::vector<sf::Sound> sfx;
	std::vector<sf::Music> bgm;
	
	sf::Sound& get_available_sound();
	
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
