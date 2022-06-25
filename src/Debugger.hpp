#pragma once

#include <SFML/Graphics.hpp>

#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Program.hpp"
#include "Visual.hpp"
#include "Input.hpp"
#include "Sound.hpp"

class PTCSystem;

#include <memory>

/// Debug console, used for the debugger to have some form of nice display.
class DebugConsole : public BaseConsole<char, PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT>, public sf::Drawable {
	/// TileMap used for rendering.
	mutable TileMap tm{32,24};
	
public:
	/// Default constructor
	DebugConsole() = default;
	
	/// Copy constructor (deleted)
	DebugConsole(const DebugConsole&) = delete;
	
	/// Copy assignment (deleted)
	DebugConsole& operator=(const DebugConsole&) = delete;
	
	/// Prints a string. Defaults to printing with a newline, unlike BaseConsole.
	/// 
	/// @param str Text to print
	/// @param newl If true, prints a newline after str.
	void print(std::string str, bool newl = true) { BaseConsole::print(str, newl); }
	
	/// Draws this DebugConsole.
	/// @param target SFML RenderTarget (usually a RenderWindow here)
	/// @param rs Render state object.
	void draw(sf::RenderTarget& target, sf::RenderStates rs) const override;
};

/// Debugger system. 
/// Runs in sync with (immediately after) PTCSystem in current implementation.
/// Offers basic debugging tools like breakpoints and tracking variables
class Debugger {
	/// Evaluator object
	std::shared_ptr<Evaluator> evaluator;
	/// Resources object
	std::shared_ptr<Resources> resources;
	/// Program object. Used to set breakpoints.
	std::shared_ptr<Program> program;
	/// Visual object. Used to share graphics resources.
	std::shared_ptr<Visual> visual;
	/// Input object
	std::shared_ptr<Input> input;
	/// Sound object
	std::shared_ptr<Sound> sound;
	
	/// Window for DebugConsole to display information to
	sf::RenderWindow window;
	/// Where debug information is printed to
	DebugConsole console;
	
	/// Determines whether or not the command will be executed.
	bool submit = false;
	/// Buffer to contain input command
	std::string command;
	/// Error message string
	std::string feedback;
	
	/// Expressions to track
	std::vector<std::string> expression;
	/// Tokenized expressions to track
	std::vector<Expr> evaluable{};
	
public:
	/// Constructor
	/// @param ptc PTCSystem to track members of
	Debugger(PTCSystem* ptc);
	
	/// Default constructor (deleted)
	Debugger() = delete;
	
	/// Copy constructor (deleted)
	Debugger(const Debugger&) = delete;
	
	/// Copy assignment (deleted)
	Debugger& operator=(const Debugger&) = delete;
	
	/// Update function. Manages window events, user input, and rendering.
	void update();
};
