#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"

#include <atomic>
#include <mutex>
#include <map>
#include <queue>

#include <SFML/Graphics.hpp>

typedef sf::Keyboard::Key Key;
typedef std::pair<Key, int> kc;

struct Input {
	Evaluator& e;
	
	std::map<Key, int> code_to_button;
	std::map<int, int> joy_to_button;
	std::map<int, int> stick_to_button;
	float sensitivity = 50; 
	
	const std::map<Key, int> code_to_ptc{
		kc(Key::Unknown, 0),
		kc(Key::Escape, 1),
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
		kc(Key::Subtract, 12),
		kc(Key::Add, 13),
		kc(Key::Equal, 14),
		kc(Key::Backspace, 15),
		kc(Key::Q, 18),
		kc(Key::W, 19),
		kc(Key::E, 20),
		kc(Key::R, 21),
		kc(Key::T, 22),
		kc(Key::Y, 23),
		kc(Key::U, 24),
		kc(Key::I, 25),
		kc(Key::O, 26),
		kc(Key::P, 27),
		kc(Key::Multiply, 29),
		kc(Key::Tab, 32),
		kc(Key::A, 34),
		kc(Key::S, 35),
		kc(Key::D, 36),
		kc(Key::F, 37),
		kc(Key::G, 38),
		kc(Key::H, 39),
		kc(Key::J, 40),
		kc(Key::K, 41),
		kc(Key::L, 42),
		kc(Key::Semicolon, 43),
		kc(Key::LShift, 47),		
		kc(Key::Quote, 48),		
		kc(Key::Z, 49),
		kc(Key::X, 50),
		kc(Key::C, 51),
		kc(Key::V, 52),
		kc(Key::B, 53),
		kc(Key::N, 54),
		kc(Key::M, 55),
		kc(Key::Comma, 56),
		kc(Key::Period, 57),
		kc(Key::Slash, 58),
		kc(Key::Divide, 58),
		kc(Key::Enter, 60),
		kc(Key::Space, 65),
		kc(Key::Insert, 66),
		kc(Key::Delete, 67),
	};
	
	const std::string       kya{"??1234567890-+=\b$\"QWERTYUIOP@*()\t!ASDFGHJKL;:<>?'ZXCVBNM,./%\r???? ???"};
	const std::string shift_kya{"....#..&.^\\~..|\b..qwertyuiop`.[]\t.asdfghjkl..{}..zxcvbnm..?_\r.... ..."};
	
	std::mutex button_mutex;
	std::vector<std::vector<int>> button_info{};
	
	std::atomic_int old_buttons; //updates each frame
	std::atomic_int buttons; //same as above
	std::atomic_int keycode; //updates each frame, sysvar

	//sysvars
	std::atomic_bool tchst;
	std::atomic_int tchtime;
	std::atomic_int tchx;
	std::atomic_int tchy;
	
	std::mutex inkeybuf_mutex;
	std::queue<char> inkeybuffer; //syncs to keycode unless a function key is pressed, which adds a string to the buffer (and has keycode = 0.)
		
	char inkey_internal();
	
	void brepeat_(const Args&);
	Var inkey_(const Vals&);
	Var button_(const Vals&);
	Var btrig_(const Vals&);
	
public:	
	Input(Evaluator&);
	
	Input() = delete;
	
	Input(const Input&) = delete;
	
	Input& operator=(const Input&) = delete;
	
	void update(int);
	void touch(bool, int, int);
	void touch_key(int keycode);
	
	int keyboard_to_keycode(Key);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
};
