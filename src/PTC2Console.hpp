#pragma once

#include <array>

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Input.hpp"
#include "TileMap.hpp"
#include "BaseConsole.hpp"
#include "IPTCObject.hpp"

class PTCSystem;

const int PTC2_CONSOLE_WIDTH = 32;
const int PTC2_CONSOLE_HEIGHT = 24;

/// Text console.
class PTC2Console : public BaseConsole<char, PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT>,
					public IPTCObject {
	/// Containing system
	PTCSystem* system;
	/// Evaluator object
	Evaluator& e;
	/// TileMap used for rendering
	TileMap tm;
	
	/// Pointer to `CSRX` system variable
	Number* csrx;
	/// Pointer to `CSRY` system variable
	Number* csry;
	/// Pointer to `TABSTEP` system variable
	Number* tabstep;
	
	/// Check for the edge case of the cursor ending up at (31,23)
	bool inTheStupidCorner = false;
	
	bool advance() override;
	void newline() override;
	void tab() override;
	void scroll() override;
	
	/// Prints a single item. First converts to a string and then prints to the console.
	/// 
	/// @param v Value to print
	void print_(const Var& v);
	/// Prints a string to the console.
	/// 
	/// @param str Text to print
	void print_str(std::string str);
	/// Shared input code (used in both `INPUT` and `LINPUT`)
	/// 
	/// @param a Arguments (expects format of `INPUT` or `LINPUT`)
	/// @return Pair containing variable name expression and actual input string
	std::pair<std::vector<Token>, std::string> input_common(const Args& a);
	
	void cls_(const Args&);
	void print_(const Args&);
	void locate_(const Args&);
	void color_(const Args&);
	void input_(const Args&);
	void linput_(const Args&);
	Var chkchr_(const Vals&);
	
public:
	/// Constructor
	/// 
	/// @param s Containing system
	PTC2Console(PTCSystem* s);
	
	/// Default constructor (deleted)
	PTC2Console() = delete;
	
	/// Copy constructor (deleted)
	PTC2Console(const PTC2Console&) = delete;
	
	/// Copy assignment (deleted)
	PTC2Console& operator=(const PTC2Console&) = delete;
	
	/// Prints "OK" to the console. Used to signal the end of program execution.
	void ok(const Args&);
	
	/// Resets the console, as if `ACLS` had been called.
	void reset();
	
	/// Prints a string value to the console.
	/// 
	/// @param x X location
	/// @param y Y location
	/// @param v Value to print (must be of type String)
	/// @param c Color
	void print(int x, int y, Var& v, int c);
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Draws the console.
	/// 
	/// @return Drawable object
	TileMap& draw();
};
