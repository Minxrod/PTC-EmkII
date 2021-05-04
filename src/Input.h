#pragma once

#include "Vars.h"
#include "Evaluator.h"

#include <atomic>
#include <mutex>
#include <map>
#include <queue>

#include <SFML/Graphics.hpp>

typedef sf::Keyboard::Key Key;
typedef std::pair<Key, int> kc;

struct Input {
	Evaluator& e;
	
	const std::map<Key, int> code_to_ptc{
		kc(Key::Num1, 2),
		kc(Key::Num2, 3),
		kc(Key::Num3, 4),
		kc(Key::Num4, 5),
		kc(Key::Num5, 6),
		kc(Key::Num6, 7),
		kc(Key::Num7, 8),
		kc(Key::Num8, 9),
		kc(Key::Num9, 10),
		kc(Key::Num0, 11),
		kc(Key::Hyphen, 12),
		kc(Key::Add, 13),
		kc(Key::Equal, 14),
		kc(Key::Equal, 15),	
		kc(Key::Backslash, 16),
	};
	
	std::mutex button_mutex;
	std::vector<std::vector<int>> button_info{};
	
	std::atomic_int buttons; //updates each frame
	std::atomic_int keycode; //updates each frame, sysvar

	//sysvars
	std::atomic_bool tchst;
	std::atomic_int tchtime;
	std::atomic_int tchx;
	std::atomic_int tchy;
	
	std::mutex inkeybuf_mutex;
	std::queue<char> inkeybuffer; //syncs to keycode unless a function key is pressed, which adds a string to the buffer (and has keycode = 0.)
	
	Input(Evaluator&);
	
	void update(int, Key);
	void touch(bool, int, int);
	
	void brepeat_(const Args&);
	Var inkey_(const Vals&);
	Var button_(const Vals&);
	Var btrig_(const Vals&);
	
public:
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs(); 	
};
