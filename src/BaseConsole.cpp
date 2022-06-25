#include <algorithm>

template <typename charT, int width, int height>
BaseConsole<charT, width, height>::BaseConsole(){}

template <typename charT, int width, int height>
BaseConsole<charT, width, height>::~BaseConsole(){}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::cls(){
	cur_x = 0;
	cur_y = 0;
	std::fill(text.begin(), text.end(), 0);
	std::fill(fg_color.begin(), fg_color.end(), cur_fg_color);
	std::fill(bg_color.begin(), bg_color.end(), cur_bg_color);
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::putchr(charT c){
	int loc = cur_x+width*cur_y;
	text[loc] = c;
	bg_color[loc] = cur_bg_color;
	fg_color[loc] = cur_fg_color;
	
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::print(std::basic_string<charT> text, bool newl){
	for (int i = 0; i < (int)text.size(); ++i){
		char c = text[i];
		putchr(c);
		
		if(i != (int)text.size()-1 || (cur_x != width-1 || cur_y != height-1))
			advance();
	}
	if (newl)
		newline();
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::locate(int x, int y){
	cur_x = x;
	cur_y = y;
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::color(int f, int b){
	cur_fg_color = f;
	cur_bg_color = b;
}

template <typename charT, int width, int height>
charT BaseConsole<charT, width, height>::chkchr(int x, int y) const {
	if (x < 0 || x > width || y < 0 || y > height){
		throw std::runtime_error{"Attempted to read character out of range"};
	}
	
	return text[x+width*y];
}

template <typename charT, int width, int height>
bool BaseConsole<charT, width, height>::advance(){
	++cur_x;
	if (cur_x >= width){
		newline();
		return true;
	}
	return false;
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::newline(){
	cur_x = 0;
	++cur_y;
	if (cur_y >= height){
		scroll();
	}
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::tab(){
	do
	{
		putchr(0);
	} while (!advance() && cur_x % tab_size != 0);
}

template <typename charT, int width, int height>
void BaseConsole<charT, width, height>::scroll(){
	cur_x = 0;
	cur_y = height-1;
	std::copy(text.begin() + width, text.end(), text.begin());
	std::copy(fg_color.begin() + width, fg_color.end(), fg_color.begin());
	std::copy(bg_color.begin() + width, bg_color.end(), bg_color.begin());
	std::fill(text.begin() + width * (height - 1), text.end(), 0);
}
