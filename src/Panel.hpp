#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "PTC2Console.hpp"
#include "Background.hpp"
#include "PanelKeyboard.hpp"
#include "Icon.hpp"
#include "IPTCObject.hpp"

#include <array>

/// Class to manage the bottom screen specific features.
/// This includes the panel, keyboard, and icon systems.
class Panel : public IPTCObject {
	/// Width of the screen (pixels)
	const static int WIDTH = 256;
	/// Height of the screen (pixels)
	const static int HEIGHT = 192;
	
	/// Containing system
	PTCSystem* system;
	/// Evaluator object
	Evaluator& e;
	/// Lower screen console (for `PNLSTR`)
	PTC2Console c;
	/// Background of keyboard/panel
	TileMap panel_bg;
	/// Icon system
	Icon icon;
	/// Keyboard visuals and tracking
	PanelKeyboard keyboard;
	
	/// Type of panel
	int pnltype; //OFF PNL KYA KYM KYK
	
	/// Updates the function key text.
	void update_keytext();
	
	//PTC commands
	void pnltype_(const Args&);
	void pnlstr_(const Args&);
	
public:
	/// Constructor
	/// 
	/// @param s Containing system
	Panel(PTCSystem* s);
	
	/// Default constructor (deleted)
	Panel() = delete;
	
	/// Copy constructor (deleted)
	Panel(const Panel&) = delete;
	
	/// Copy assignment (deleted)
	Panel& operator=(const Panel&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Updates keyboard and icon based on touch information.
	/// 
	/// @param t Touch state (true=touching)
	/// @param x x location of touch
	/// @param y y location of touch
	void touch_keys(bool t, int x, int y);
	
	/// Returns drawable for panel background.
	/// 
	/// @return Panel tilemap
	TileMap& draw_panel();
	/// Returns drawable for keyboard keys.
	/// 
	/// @return Keyboard sprites
	SpriteArray& draw_keyboard();
	/// Returns drawable for function key text.
	/// 
	/// @return Function keys text
	TileMap& draw_funckeys();
	/// Returns drawable for icons.
	/// 
	/// @return Icon sprites
	Icon& draw_icon();
	
	/// Check the current panel type. Nonzero result implies that the panel is enabled.
	/// The possible types, in order, are [OFF=0, PNL=1, KYA=2, KYM=3, KYK=4].
	/// 
	/// @return	Panel type [0-4]
	int panel_on() { return pnltype; }
	
	/// Override a previous `PNLTYPE` to enable the keyboard.
	/// @todo Override with previously selected keyboard type (currently defaults to KYA)
	/// 
	/// @param type New panel type.
	void panel_override(int type = 2) { pnltype = type; }
	
	/// Returns the lower screen text console.
	/// 
	/// @return Console
	PTC2Console& get_console() { return c; }
	
	/// Gets the keycode of the last key pressed.
	/// 
	/// @return keycode
	int get_last_keycode();
	/// Gets the coordinates of the last key pressed.
	/// 
	/// @return (x,y) pair
	std::pair<int,int> get_last_xy();
	/// Gets the coordinates of a key based on keycode.
	/// 
	/// @param keycode Key to locate
	/// @return (x,y) pair
	std::pair<int,int> get_keycode_xy(int keycode);
};
