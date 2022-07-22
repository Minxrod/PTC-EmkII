#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "SpriteArray.hpp"
#include "Repeater.hpp"

/// Icon subsystem class. Manages the icon and button sprites and interactions.
class Icon : public sf::Drawable, public IPTCObject {
	/// Evaluator object
	Evaluator& e;
	/// Up button sprite
	SpriteInfo up;
	/// Down button sprite
	SpriteInfo down;
	/// List of icon sprites
	std::vector<SpriteInfo> sprites;
	
	//icon paging sysvars
	/// `ICONPUSE` system variable (use icon paging)
	Number* iconpuse;
	/// `ICONPMAX` system variable (max page)
	Number* iconpmax;
	/// `ICONPAGE` system variable (current page)
	Number* iconpage;
	
	/// Index of last icon/button pressed.
	/// * -1: No icon
	/// * -2,-3: Up/Down sprites
	/// * 0-3: Icon of same number
	int last_icon_pressed = -1;
	/// Timer to track icon hold timing
	Repeater last_pressed_timer;
	
	//PTC commands/functions
	void iconset_(const Args&);
	void iconclr_(const Args&);
	Var iconchk_(const Vals&);
	
public:
	/// Constructor
	/// 
	/// @param e Evaluator object
	Icon(Evaluator& e);
	
	/// Default constructor (deleted)
	Icon() = delete;
	
	/// Copy constructor (deleted)
	Icon(const Icon&) = delete;
	
	/// Copy assignment (deleted)
	Icon& operator=(const Icon&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Updates icons pressed based on touch data
	/// 
	/// @param t Touch state (true=touching screen)
	/// @param x x location of touch
	/// @param y y location of touch
	void update(bool t, int x, int y);
	/// Draws the icons and buttons.
	/// 
	/// @param target Target to render to
	/// @param rs Render states
	void draw(sf::RenderTarget& target, sf::RenderStates rs) const override;
};
