#pragma once

#include <array>
#include <string>

//
// Basic text console class. Supports color and text.
//
//
template <typename charT, int width, int height>
class BaseConsole {
	const int WIDTH = width;
	const int HEIGHT = height; 
	
	char cur_fg_color = 0;
	char cur_bg_color = 0;
	int cur_x = 0;
	int cur_y = 0;
	int tab_size = 4;
	
	std::array<unsigned char, width*height> bg_color{};
	std::array<unsigned char, width*height> fg_color{};
	std::array<charT, width*height> text{};
	
protected:
	bool advance();
	void newline();
	void tab();
	void scroll();
	
public:
	BaseConsole();
	
	BaseConsole(const BaseConsole&) = delete;
	
	BaseConsole& operator=(const BaseConsole&) = delete;
	
	virtual ~BaseConsole();
	
	void cls();
	void putchr(charT c);
	void print(std::basic_string<charT> t, bool newl = false);
	void locate(int x, int y);
	void color(int f, int b = 0);
	charT chkchr(int x, int y) const;
	
	int get_w() const { return WIDTH; }
	int get_h() const { return HEIGHT; }
	int get_x() const { return cur_x; }
	int get_y() const { return cur_y; }
	int get_tab() const { return tab_size; }
	int get_fg() const { return cur_fg_color; }
	int get_bg() const { return cur_bg_color; }
	int get_fg(int x, int y) const { return fg_color[x + width * y]; }
	int get_bg(int x, int y) const { return bg_color[x + width * y]; }
	
	void set_tab(int t) { tab_size = t; }
};

#include "BaseConsole.cpp"
