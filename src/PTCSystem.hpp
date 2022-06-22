#pragma once

#include <SFML/Graphics.hpp>

#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Program.hpp"
#include "Visual.hpp"
#include "Input.hpp"
#include "Sound.hpp"

#include <memory>
#include <fstream>

class PTCSystem {
	//Variety of components needed for the system to work
	std::shared_ptr<Evaluator> evaluator;
	std::shared_ptr<Resources> resources;
	std::shared_ptr<Program> program;
	std::shared_ptr<Visual> visual;
	std::shared_ptr<Input> input;
	std::shared_ptr<Sound> sound;

	//Window and keyboard control
	std::vector<sf::Keyboard::Key> special{};
	bool keybutton_enable = true; // Use keyboard as controller
	bool keyboard_enable = true; // Use keyboard to simulate touch keyboard
	
	//Display
	sf::RenderWindow window;

	//touch control
	bool mouse_press = false;
	int mouse_time = 0;
	int mouse_x = 0;
	int mouse_y = 0;

	
public:
	PTCSystem();
	
	PTCSystem(const PTCSystem&) = delete;
	
	PTCSystem& operator=(const PTCSystem&) = delete;
	
	
	void set_option(std::string option, int state);
	
	// Runs whatever program is loaded.
	void start(){
		program->run();
	}
	
	bool is_ok(){
		return window.isOpen();
	}
	
	void update();
};
