#pragma once

#include "Vars.hpp"
#include "Resources.hpp"
#include "IPTCObject.hpp"

class Visual;

class Graphics : public IPTCObject {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;	
	
	Evaluator& e;
	Resources& r;
	Visual* v;
	std::map<std::string, GRP>& grp;
	
	unsigned char gcolor;
	bool gdrawmd = false;
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
	
	Var gspoit_(const Vals&);
	
	void draw_pixel(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x, const int y, const int c);
	void draw_line(std::array<unsigned char, 256*192*4>& i, std::vector<unsigned char>& g, const int x1, const int y1, const int x2, const int y2, const int c);
	
	std::array<std::array<unsigned char, WIDTH*HEIGHT*4>, 4> image;
	
public:
	Graphics(Evaluator&, std::map<std::string, GRP>&, Resources&, Visual*);
	
	Graphics() = delete;
	
	Graphics(const Graphics&) = delete;
	
	Graphics& operator=(const Graphics&) = delete;
	
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	void reset();
	
	int get_prio(int);
	std::array<unsigned char, WIDTH*HEIGHT*4>& draw(int);
};
