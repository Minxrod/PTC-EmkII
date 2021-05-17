#pragma once

#include "Vars.h"
#include "TileMap.h"
#include "Evaluator.h"
#include "Resources.h"

struct BGInfo {
	double x;
	double y;
	double xstep;
	double ystep;
	int time;
};

class Background {
	Evaluator& e;
	std::map<std::string, SCR> scr;
	std::vector<TileMap> bg_layers;
	std::vector<BGInfo> bg_info;
	
	int page = 0;
	
	int bgclip_x1 = 0;
	int bgclip_y1 = 0;
	int bgclip_x2 = 31;
	int bgclip_y2 = 23;
	
	void bgpage_(const Args&);
	void bgclr_(const Args&);
	void bgclip_(const Args&);
	void bgofs_(const Args&);
	void bgput_(const Args&);
	void bgfill_(const Args&);
	void bgread_(const Args&);
	void bgcopy_(const Args&);
	
	Var bgchk_(const Vals&);
	
public:
	Background(Evaluator&, std::map<std::string, SCR>&);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	TileMap& draw(int, int);
};
