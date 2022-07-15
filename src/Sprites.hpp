#pragma once

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "SpriteArray.hpp"
#include "IPTCObject.hpp"

/// Manages the sprites for both screens.
class Sprites : public IPTCObject {
	/// Evaluator object
	Evaluator& e;
	/// List of sprites for each screen
	std::vector<std::vector<SpriteInfo>> sprites;
	/// Current sprite screen
	int page;
	
	void sppage_(const Args&);
	void spset_(const Args&);
	void spclr_(const Args&);
	void sphome_(const Args&);
	
	void spofs_(const Args&);
	void spchr_(const Args&);
	void spanim_(const Args&);
	void spangle_(const Args&);
	void spscale_(const Args&);
	
	Var spchk_(const Vals&);
	void spread_(const Args&);
	void spsetv_(const Args&);
	Var spgetv_(const Vals&);
	
	void spcol_(const Args&);
	void spcolvec_(const Args&);
	Var sphit_(const Vals&);
	Var sphitsp_(const Vals&);
	Var sphitrc_(const Vals&);
	
public:
	/// Constructor
	///
	/// @param ev Evaluator
	Sprites(Evaluator& ev);
	
	/// Default constructor (deleted)
	Sprites() = delete;
	
	/// Copy constructor (deleted)
	Sprites(const Sprites&) = delete;
	
	/// Copy assignment (deleted)
	Sprites& operator=(const Sprites&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Resets the sprites, as if ACLS was called.
	void reset();
	/// Advances sprites by one frame.
	void update();
	/// Draws the sprites corresponding to the given screen and priority.
	/// 
	/// @param page Screen to draw
	/// @param prio Priority of sprites to draw
	/// @return SpriteArray containing sprites to be rendered
	SpriteArray draw(int page, int prio);
	
	/// Gets the current sprite screen.
	/// 
	/// @return Current sprite page.
	int get_page() { return page; }
};
