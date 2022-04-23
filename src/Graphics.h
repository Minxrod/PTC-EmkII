#pragma once

#include "Vars.h"
#include "Resources.h"

class Graphics {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;	
	
	Evaluator& e;
	std::map<std::string, GRP>& grp;
	
	unsigned char gcolor;
	bool gdrawmd;
	int screen;
	int drawpage[2];
	int displaypage[2];
	int prio[2];
	
	int xy_to_chr(int, int);
	
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
	
	Var gspoit(const Vals&);
	
	std::array<std::array<unsigned char, WIDTH*HEIGHT*4>, 4> image;
	
public:
	Graphics(Evaluator&, std::map<std::string, GRP>&);
	
	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void reset();
	
	int get_prio(int);
	std::array<unsigned char, WIDTH*HEIGHT*4>& draw(int);
};
