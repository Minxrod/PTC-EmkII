#pragma once

#include "Vars.hpp"
#include "Sprites.hpp"
#include "SpriteArray.hpp"
#include "TileMap.hpp"
#include "Repeater.hpp"

/// Manages the keyboard and function key graphics.
class PanelKeyboard {
	/// Function key text for rendering
	TileMap func_keys;
	/// Strings containing full text of function keys
	std::vector<std::string> func_text;
	/// Keyboard key sprites
	std::vector<SpriteInfo> keys;
	
	/// Pointers to all component sprites of the spacebar.
	std::vector<SpriteInfo*> space;
	/// Pointers to all component sprites of the RUN key.
	std::vector<SpriteInfo*> run;
	/// Pointers to all component sprites of the EDIT key.
	std::vector<SpriteInfo*> edit;
	/// Pointers to all function key component sprites.
	std::vector<std::vector<SpriteInfo*>> func;
	
	/// Pointer to last pressed key's sprite data.
	SpriteInfo* last_pressed;
	/// Key hold timer
	Repeater last_pressed_timer;
	
	/// Renderable key sprites.
	SpriteArray key_sp;
	
	/// Updates the text on the function keys.
	void update_keytext();
	/// Updates a key to either pressed or unpressed position, depending on the value of dir.
	/// 
	/// @param key Key to update
	/// @param dir Direction to offset sprite
	void update_key(SpriteInfo& key, int dir);
	
public:
	/// Default constructor
	PanelKeyboard();
	
	/// Copy constructor (deleted)
	PanelKeyboard(const PanelKeyboard&) = delete;
	
	/// Copy assignment (deleted)
	PanelKeyboard& operator=(const PanelKeyboard&) = delete;
	
	/// Check for key presses and update keys accordingly.
	/// 
	/// @param t Is the touch screen being touched?
	/// @param x x component of touch location
	/// @param y y component of touch location
	void touch_keys(bool t, int x, int y);
	/// Returns a drawable object for the keyboard.
	/// 
	/// @return Keyboard keys drawable
	SpriteArray& draw_keyboard();
	/// Returns a drawable object for the function key text.
	/// 
	/// @return Function key text drawable
	TileMap& draw_funckeys();
	
	/// Returns the keycode of the currently pressed key.
	/// 
	/// @return Current keycode
	int get_last_keycode();
	/// Returns the coordinates of a key pressed based on the keycode.
	///
	/// @param keycode
	/// @return std::pair containing (x,y)
	std::pair<int, int> get_keycode_xy(int keycode);
	/// Returns the x,y coordinates of the last key pressed..
	/// 
	/// @return std::pair containing (x,y)
	std::pair<int, int> get_last_xy();
};
