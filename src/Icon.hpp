#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "SpriteArray.hpp"

class Icon : public sf::Drawable, public IPTCObject {
	Evaluator& e;
	SpriteInfo up;
	SpriteInfo down;
	std::vector<SpriteInfo> sprites;
	
	//icon paging sysvars
	Number* iconpuse;
	Number* iconpmax;
	Number* iconpage;
	
	int last_icon_pressed = -1;
	
	void iconset_(const Args&);
	void iconclr_(const Args&);
	Var iconchk_(const Vals&);
	
public:
	Icon(Evaluator& e);
	
	Icon() = delete;
	
	Icon(const Icon&) = delete;
	
	Icon& operator=(const Icon&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	void update(bool touch, int x, int y);
	void draw(sf::RenderTarget& target, sf::RenderStates rs) const override;
};
