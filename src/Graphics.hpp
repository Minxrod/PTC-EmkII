#pragma once

#include "Vars.hpp"
#include "Resources.hpp"
#include "IPTCObject.hpp"

class Visual;

/// Manages the GRP pages and assocatied information.
/// 
class Graphics : public IPTCObject {
	/// Width of screen, in pixels
	const static int WIDTH = 256;
	/// Height of screen, in pixels
	const static int HEIGHT = 192;	
	
	/// Evaluator object.
	Evaluator& e;
	/// Resources object. Needed for GPUTCHR to access CHR data.
	Resources& r;
	/// Pointer to containing Visual object. Needed for GPUTCHR to update the color palette.
	Visual* v;
	/// GRP pages.
	std::map<std::string, GRP>& grp;
	
	/// Current color for drawing commands.
	unsigned char gcolor;
	/// XOR drawing mode (pixel color is XOR'd with old color instead of overwriting)
	bool gdrawmd = false;
	/// Current GPAGE screen
	int screen;
	/// Drawing pages (which GRP do commands write to)
	int drawpage[2];
	/// Display pages (what is currently on the screen)
	int displaypage[2];
	/// Display priority of GRP page
	int prio[2];
	
	// PTC commands
	void gcolor_(const Args&);
	void gcls_(const Args&);
	void gpage_(const Args&);
	void gpset_(const Args&);
	void gline_(const Args&);
	void gcircle_(const Args&);
	void gpaint_(const Args&);
	void gbox_(const Args&);
	void gfill_(const Args&);
	void gputchr_(const Args&);
	void gdrawmd_(const Args&);
	void gprio_(const Args&);
	void gcopy_(const Args&);
	
	// PTC functions
	Var gspoit_(const Vals&);
	
	/// Draws a single pixel at the given coordinates
	/// 
	/// @param i Display array to write to 
	/// @param g GRP data to write to
	/// @param x x coordinate to write to
	/// @param y y coordinate to write to
	/// @param c Color to write (or XOR with old color, if gdrawmd is true)
	void draw_pixel(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x, const int y, const int c);
	
	/// Draws a line of pixels between two points.
	/// @note Not exactly the same as PTC `GLINE`.
	/// 
	/// @param i Display array to write to 
	/// @param g GRP data to write to
	/// @param x1 x coordinate to start at
	/// @param y1 y coordinate to start at
	/// @param x2 x coordinate to end at
	/// @param y2 y coordinate to end at
	/// @param c Color to write (or XOR with old color, if gdrawmd is true)
	void draw_line(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x1, const int y1, const int x2, const int y2, const int c);
	
	/// Array of image texture arrays, one for each GRP.
	std::array<std::array<unsigned char, WIDTH*HEIGHT*4>, 4> image;
	
public:
	/// Constructor
	/// 
	/// @param ev Evaluator
	/// @param grps Map of GRP objects
	/// @param res Resources object
	/// @param vis Pointer to Visual object
	Graphics(Evaluator& ev, std::map<std::string, GRP>& grps, Resources& res, Visual* vis);
	
	/// Default constructor (deleted)
	Graphics() = delete;
	
	/// Copy constructor (deleted)
	Graphics(const Graphics&) = delete;
	
	/// Copy assignment (deleted)
	Graphics& operator=(const Graphics&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Resets this object, as if `ACLS` had been called.
	void reset();
	
	/// Gets the draw priority of the screen.
	/// 
	/// @param screen Screen to check priority of
	int get_prio(int screen);
	
	/// Gets the texture array of the display page of the screen.
	/// 
	/// @param screen Screen to get texture array of
	std::array<unsigned char, WIDTH*HEIGHT*4>& draw(int screen);
	
	/// Gets the current `GPAGE` screen.
	/// 
	/// @return Current graphics screen
	int get_page() { return screen; };
};
