#pragma once

#include "Vars.hpp"
#include "TileMap.hpp"
#include "Evaluator.hpp"
#include "Resources.hpp"
#include "IPTCObject.hpp"

///
/// Background management class. Controls the background layers and implements all the BG* commands.
///
class Background : public IPTCObject {
	///
	///Struct defining position and animation data for a BG layer.
	///
	struct BGInfo {
		/// Current X position (pixels)
		double x;
		/// Current Y position (pixels)
		double y;
		/// Change in X per frame (for animation)
		double xstep;
		/// Change in Y per frame (for animation)
		double ystep;
		/// Time remaining (frames) (for animation)
		int time;
	};
	
	int bgclip_x1 = 0;
	int bgclip_y1 = 0;
	int bgclip_x2 = 31;
	int bgclip_y2 = 23;
	
	/// Evaluator object
	Evaluator& e;
	/// SCR objects (contains all actual tile data)
	std::map<std::string, SCR> scr;
	/// TileMap objects (used for rendering)
	std::vector<TileMap> bg_layers;
	/// BGInfo objects (position and animation data)
	std::vector<BGInfo> bg_info;
	/// Current BG page (0=lower screen, 1=upper screen)
	int page = 0;
	
	// PTC commands
	void bgpage_(const Args&);
	void bgclr_(const Args&);
	void bgclip_(const Args&);
	void bgofs_(const Args&);
	void bgput_(const Args&);
	void bgfill_(const Args&);
	void bgread_(const Args&);
	void bgcopy_(const Args&);
	
	// PTC functions
	Var bgchk_(const Vals&);
	
public:
	///
	/// Creates the Background manager. Controls both upper and lower screen 
	///
	/// @param ev Evaluator object
	/// @param s Map of resource name to SCR objects
	///
	Background(Evaluator&, std::map<std::string, SCR>&);
	
	/// Default constructor (deleted)
	Background() = delete;
	
	/// Copy constructor (deleted)
	Background(const Background&) = delete;
	
	/// Copy assignment (deleted)
	Background& operator=(const Background&) = delete;
	
	/// Creates and returns a map of PTC command tokens to callable commands for this object.
	/// @return Map of Tokens to callable objects
	std::map<Token, cmd_type> get_cmds() override;
	
	/// Creates and returns a map of PTC function tokens to callable functions for this object.
	/// @return Map of Tokens to callable objects
	std::map<Token, op_func> get_funcs() override;
	
	/// Resets the background state, as if ACLS had been called.
	void reset();
	
	/// Advances one frame. (Used for updating animation)
	void update();
	
	/// Returns the drawable TileMap corresponding to the given screen and layer.
	/// @note No actual drawing takes place in this function. This function only updates the position of the TileMap as set from BGOFS.
	///
	/// @param screen Screen (0=top, 1=bottom)
	/// @param layer Layer (0=foreground, 1=background)
	/// @return Tilemap for the screen and layer.
	TileMap& draw(int screen, int layer);
	
	/// Gets the current BG page.
	int get_page() { return page; }
	
	sf::View get_clip(int sc);
};
