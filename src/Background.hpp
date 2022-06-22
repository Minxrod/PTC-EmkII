#pragma once

#include "Vars.hpp"
#include "TileMap.hpp"
#include "Evaluator.hpp"
#include "Resources.hpp"

/*
Struct defining position and animation data for a BG layer.
*/
struct BGInfo {
	// Current X position
	double x;
	// Current Y position
	double y;
	// Change in X per frame (for animation)
	double xstep;
	// Change in Y per frame (for animation)
	double ystep;
	// Time remaining (for animation)
	int time;
};

/*
Background management class. Controls the background layers and implements
all the BG* commands.
*/
class Background {
	// Evaluator object
	Evaluator& e;
	// SCR objects (contains all actual tile data)
	std::map<std::string, SCR> scr;
	// TileMap objects (used for rendering)
	std::vector<TileMap> bg_layers;
	// BGInfo objects (position and animation data)
	std::vector<BGInfo> bg_info;
	// Current BG page (0=lower screen, 1=upper screen)
	int page = 0;
	
	// BG clipping boundaries (default is entire screen)
	// TODO: these don't need to be class values, there seems to be no way to read these?
	int bgclip_x1 = 0;
	int bgclip_y1 = 0;
	int bgclip_x2 = 31;
	int bgclip_y2 = 23;
	
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
	Background(Evaluator&, std::map<std::string, SCR>&);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void reset();
	void update();
	TileMap& draw(int, int);
};
