#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"

#include <atomic>
#include <mutex>
#include <map>
#include <queue>
#include <string>

#include <SFML/Graphics.hpp>

typedef sf::Keyboard::Key Key;
typedef std::pair<Key, int> kc;

using namespace std::string_literals;

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
	
	/// INKEY$ characters per keyboard.
	const std::vector<std::string> kyx{
		"\0\0001234567890-+=\0$\"QWERTYUIOP@*()\t!ASDFGHJKL;:<>\0'ZXCVBNM,./%\r\0\0\0\0 \0\0\0"s, //kya
		"\0\0\0\0#\0\0&\0^\\~\0\x7f|\0\0\0qwertyuiop`\0[]\t\0asdfghjkl\0\0{}\0\0zxcvbnm\0\0?_\r\0\0\0\0 \0\0\0"s, //shift_kya
		"\0\0\xF1\xF2\xF0\xF3\x0A\xE5\x1A\xE4\xE6\xE7\x10\x07\x14\0\x03\x96\x98\x91\x99\xEC\xEE\xEF\xED\x0E\x0F\x05\x06"s+
		"\x04\x0B\x1B\t\x95\x92\x93\x94\x15\x17\x16\xF4\xF6\xF7\xF5\xEB\xEA\xE8\0\xFF\x9A\x90\x9B\xFC\xFD\xFE\x97\x9C\x9D\x9E\x9F\r\0\0\0\0 \0\0\0"s, //kigou
		"\0\0\0\x80\0\0\0\xE1\0\xE0\xE2\xE3\x11\x08\0\0\0\0\xF8\x1E\xFA\x01\x02\x18\x19\x0C\x12\x13\0\0\xE9\0\t"s+
		"\0\x1D\0\x1C\0\x81\x82\x83\x84\x85\x86\x87\0\0\0\0\xF9\x1F\xFB\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\r\0\0\0\0 \0\0\0"s, //shift_kigou
		"\0\0\xB1\xB2\xB3\xB4\xB5\xC5\xC6\xC7\xC8\xC9\x2D\x2B\x3D\0\xA0\xA5\xB6\xB7\xB8\xB9\xBA\xCA\xCB\xCC\xCD\xCE\xA2"s+
		"\xA3\xDD\xA1\t\xBB\xBC\xBD\xBE\xBF\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xA4\0\xC0\xC1\xC2\xC3\xC4\xD7\xD8\xD9\xDA\xDB\xDC\xA6\r\0\0\0\0 \0\0\0"s, //kana
		"\0 \0 \xA7\0\xA8\0\xA9\0\xAA\0\xAB\0\0 \0 \0 \0 \0 \xCA\xDF\xCB\xDF\xCC\xDF\0 "s+
		"\xAF\0\xB3\xDE\xB6\xDE\xB7\xDE\xB8\xDE\xB9\xDE\xBA\xDE\xCA\xDE\xCB\xDE\xCC\xDE\xCD\xDE\xCE\xDE\xCD\xDF\xCE\xDF\0 \0 "s+
		"\t\0\xBB\xDE\xBC\xDE\xBD\xDE\xBE\xDE\xBF\xDE\x31\0\x32\0\x33\0\x34\0\x35\0\xAC\0\xAD\0\xAE\0\0 "s+
		"\0 \xC0\xDE\xC1\xDE\xC2\xDE\xC3\xDE\xC4\xDE\x36\0\x37\0\x38\0\x39\0\x30\0\0 \0 \r\0\0 \0 \0 \0  \0\0 \0 \0 "s //shift_kana
	};
	
	/// Current active keyboard.
	int keyboard = 0;
	/// Whether or not SHIFT should be active. SHIFT=1, CAPS LOCK=2
	int shift = 0;
	
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
	wchar_t inkey_internal();
	
	/// Only really needed for special keys. Zeroes value after use.
	///
	/// @return `KEYBOARD` value or 0
	int keycode_internal();
	
	/// Internal function to type characters.
	/// 
	/// @param keys String to type
	void type_try_keyboard(std::string keys, int);
	
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
	
	/// Maps a unicode character to the matching PTC character if possible, and writes it to the `INKEY$()` buffer.
	/// 
	/// @param unicode Character code
	/// @param key sf::Key SFML keycode
	void type(int unicode, Key key);
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
};
