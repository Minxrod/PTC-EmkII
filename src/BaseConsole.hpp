#pragma once

#include <array>
#include <string>

/// Basic text console class. Supports color and text.
///
/// This class is intended to represent a generic text console, as may be seen in PTC, PTC mkII, or SmileBASIC 3.
/// This class only implements the text and color functionality. This does not implement any of the rendering.
/// Those functions are intended to be handled by subclasses.
///
template <typename charT, int width, int height>
class BaseConsole {
	/// Width of the text console, in characters.
	const int WIDTH = width;
	/// Height of the text console, in characters.
	const int HEIGHT = height; 
	
	/// Current text color for newly printed text.
	char cur_fg_color = 0;
	/// Current background color for newly printed text.
	char cur_bg_color = 0;
	/// Cursor location x component.
	int cur_x = 0;
	/// Cursor location y component.
	int cur_y = 0;
	/// Size of tab in characters, for use by tab().
	int tab_size = 4;
	
	/// Array containing background color of text printed to the console.
	std::array<unsigned char, width*height> bg_color{};
	/// Array containing color of text printed to the console.
	std::array<unsigned char, width*height> fg_color{};
	/// Array containing characters printed to the console.
	std::array<charT, width*height> text{};
	
protected:
	/// Advances the cursor by one pixel. Will scroll console if reaches the end of the line.
	/// @return true if console was scrolled.
	virtual bool advance();
	/// Advances the cursor to the next line.
	virtual void newline();
	/// Advances the cursor to the next tab break, as determined by tab_size.
	virtual void tab();
	/// Scrolls up the console by one line.
	virtual void scroll();
	
public:
	/// Constructor.
	BaseConsole();
	
	/// Copy constructor (deleted)
	BaseConsole(const BaseConsole&) = delete;
	
	/// Copy assignment (deleted)
	BaseConsole& operator=(const BaseConsole&) = delete;
	
	/// Destructor
	virtual ~BaseConsole();
	
	/// Clears the console.
	/// @note This function resets the cursor location, but not the current colors.
	void cls();
	
	/// Places a single character to the console.
	/// @param c Character code
	void putchr(charT c);
	
	/// Prints a string of the given character type to the console, at the current cursor location and with the current colors.
	/// @param text Text string to print
	/// @param newl If true, will automatically call newline() when done printing
	void print(std::basic_string<charT> text, bool newl = false);
	
	/// Sets the current cursor location.
	/// @note Does not currently perform any bounds checks.
	/// @param x Cursor x location
	/// @param y Cursor y location
	void locate(int x, int y);
	
	/// Sets the current text color and background color.
	/// @note Does not currently perform any bounds checks.
	/// @param f Text foreground color
	/// @param b Text background color
	void color(int f, int b = 0);
	
	/// Gets the character at the given location.
	/// @param x Console x location
	/// @param y Console y location
	/// @return Character code at (x,y)
	charT chkchr(int x, int y) const;
	
	/// Gets the console's width.
	/// @return Console width
	int get_w() const { return WIDTH; }

	/// Gets the console's height.
	/// @return Console height
	int get_h() const { return HEIGHT; }
	
	/// Gets the cursor's x location.
	/// @return Cursor x
	int get_x() const { return cur_x; }
	
	/// Gets the cursor's y location.
	/// @return Cursor y
	int get_y() const { return cur_y; }
	
	/// Gets the current tab width.
	/// @return Tab width
	int get_tab() const { return tab_size; }
	
	/// Gets the current text foreground color.
	/// @return Current text color
	int get_fg() const { return cur_fg_color; }
	
	/// Gets the current text background color.
	/// @return Current background color
	int get_bg() const { return cur_bg_color; }
	
	/// Gets the text foreground color at the provided location.
	/// @param x Console x location
	/// @param y Console y location
	/// @return Current text color
	int get_fg(int x, int y) const { return fg_color[x + width * y]; }
	
	/// Gets the text background color at the provided location.
	/// @param x Console x location
	/// @param y Console y location
	/// @return Current background color
	int get_bg(int x, int y) const { return bg_color[x + width * y]; }
	
	/// Sets the current tab width.
	/// @param t New tab width
	void set_tab(int t) { tab_size = t; }
};

#include "BaseConsole.cpp"
