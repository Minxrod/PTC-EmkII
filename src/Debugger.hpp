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

class DebugConsole : public BaseConsole<char, PTC2_CONSOLE_WIDTH, PTC2_CONSOLE_HEIGHT>, public sf::Drawable {
	mutable TileMap tm{32,24};
	
public:
	DebugConsole() = default;
	
	DebugConsole(const DebugConsole&) = delete;
	
	DebugConsole& operator=(const DebugConsole&) = delete;
	
	void print(std::string str, bool newl = true) { BaseConsole::print(str, newl); }
	
	void draw(sf::RenderTarget& target, sf::RenderStates rs) const override;
};

class Debugger {
	//Variety of components to watch
	std::shared_ptr<Evaluator> evaluator;
	std::shared_ptr<Resources> resources;
	std::shared_ptr<Program> program;
	std::shared_ptr<Visual> visual;
	std::shared_ptr<Input> input;
	std::shared_ptr<Sound> sound;
	
	//Display
	sf::RenderWindow window;
	DebugConsole console;
	
	//Buffer to contain input command
	bool submit = false;
	std::string command;
	
	//Expressions to track
	std::vector<std::string> expression;
	std::vector<Expr> evaluable{};
	
public:
	Debugger(PTCSystem* ptc);
	
	Debugger() = delete;
	
	Debugger(const Debugger&) = delete;
	
	Debugger& operator=(const Debugger&) = delete;
	
	void update();
};
