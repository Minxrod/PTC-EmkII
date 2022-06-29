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

/// Input management struct.
/// 
/// Provides access to PTC input functionss, and additionally manages part of the keyboard input.
struct Input : public IPTCObject {
	/// Evaluator object
	Evaluator& e;
	
	/// Map of keyboard key codes to button codes
	std::map<Key, int> code_to_button;
	/// Map of controller buttons to button codes
	std::map<int, int> joy_to_button;
	/// Map of controller sticks to d-pad button codes
	std::map<int, int> stick_to_button;
	/// Stick sensitivity for controllers
	float sensitivity = 50;
	
	/// Map of keyboard keys to PTC `KEYBOARD` key codes
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
	
	/// Keyboard characters list for default keyboard
	const std::string       kya{"??1234567890-+=\b$\"QWERTYUIOP@*()\t!ASDFGHJKL;:<>?'ZXCVBNM,./%\r???? ???"};
	/// Keyboard characters list for lowercase keyboard (unused)
	const std::string shift_kya{"....#..&.^\\~..|\b..qwertyuiop`.[]\t.asdfghjkl..{}..zxcvbnm..?_\r.... ..."};
	
	/// Mutex for updating button_info
	std::mutex button_mutex;
	/// 2D array containing button repeat timings and current hold duration.
	std::vector<std::vector<int>> button_info{};
	
	/// Button status from previus frame
	std::atomic_int old_buttons; //updates each frame
	/// Current button status
	std::atomic_int buttons; //same as above
	/// Current keycode
	std::atomic_int keycode; //updates each frame, sysvar

	//sysvars
	/// TCHST sysvar (touch state)
	std::atomic_bool tchst;
	/// TCHTIME sysvar (hold timer)
	std::atomic_int tchtime;
	/// TCHX sysvar
	std::atomic_int tchx;
	/// TCHY sysvar
	std::atomic_int tchy;
	
	/// Mutex for INKEY$() buffer
	std::mutex inkeybuf_mutex;
	/// Buffer for characters that need to be typed
	std::queue<char> inkeybuffer; //syncs to keycode unless a function key is pressed, which adds a string to the buffer (and has keycode = 0.)
	
	/// Returns a character from the INKEY$() buffer
	/// 
	/// @return character typed
	char inkey_internal();
	
	// PTC commands
	void brepeat_(const Args&);
	
	// PTC functions
	Var inkey_(const Vals&);
	Var button_(const Vals&);
	Var btrig_(const Vals&);
	
public:
	/// Constructor
	///
	/// @param ev Evaluator object
	Input(Evaluator& ev);
	
	/// Default constructor (deleted)
	Input() = delete;
	
	/// Copy constructor (deleted)
	Input(const Input&) = delete;
	
	/// Copy assignment (deleted)
	Input& operator=(const Input&) = delete;
	
	/// Sets the buttons currently pressed.
	/// 
	/// @param b BUTTON() equivalent data
	void update(int b);
	/// Sets the touchscreen state.
	/// 
	/// @param t Touch state
	/// @param x Touch x coordinate
	/// @param y Touch y coordinate
	void touch(bool t, int x, int y);
	/// Simulates the touch of the given key on the current keyboard.
	/// 
	/// @param keycode Location of key pressed
	void touch_key(int keycode);
	
	/// Converts a sf::Keyboard::Key to a `KEYBOARD` keycode.
	/// 
	/// @param SFML keycode
	/// @return PTC keycode
	int keyboard_to_keycode(Key);
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
};
