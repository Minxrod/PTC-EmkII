#pragma once

#include <SFML/Graphics.hpp>

#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Program.hpp"
#include "Visual.hpp"
#include "Input.hpp"
#include "Sound.hpp"
#include "Debugger.hpp"

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
	
public:
	PTCSystem();
	
	PTCSystem(const PTCSystem&) = delete;
	
	PTCSystem& operator=(const PTCSystem&) = delete;
	
	virtual ~PTCSystem(){}
	
	void set_option(std::string option, int state);
	
	/// Loads a new program, updating all resources required.
	void load_program(std::string filename);
	
	/// Runs whatever program is loaded.
	void start(){
		program->run();
	}
	
	/// Check if program is currently running.
	/// TODO: Check if this is correct
	bool is_running(){
		return !program->at_eof();
	}
	
	std::shared_ptr<Resources> get_resources(){
		return resources;
	}
	
	std::shared_ptr<Evaluator> get_evaluator(){
		return evaluator;
	}
	
	std::shared_ptr<Program> get_program(){
		return program;
	}
	
	std::shared_ptr<Visual> get_visual(){
		return visual;
	}
	
	std::shared_ptr<Input> get_input(){
		return input;
	}
	
	std::shared_ptr<Sound> get_sound(){
		return sound;
	}
	
	/// Run each frame to update time based events.
	void update();
};

/// Class that creates and displays a PTCSystem
class PTCSystemDisplay : public PTCSystem {
	//Extra for debugging
	std::unique_ptr<Debugger> debug;
	
	//Display
	sf::RenderWindow window;
	
	//Window and keyboard control
//	std::vector<sf::Keyboard::Key> special{};
	bool keybutton_enable = true; // Use keyboard as controller
	bool keyboard_enable = true; // Use keyboard to simulate touch keyboard

	//touch control
	bool mouse_press = false;
	int mouse_time = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	
public:
	/// Default constructor
	PTCSystemDisplay();
	
	/// Copy constructor (deleted)
	PTCSystemDisplay(const PTCSystemDisplay&) = delete;
	
	/// Copy assignment (deleted)
	PTCSystemDisplay& operator=(const PTCSystemDisplay&) = delete;
	
	void set_option(std::string option, int state);
	
	bool is_ok(){
		return window.isOpen();
	}
	
	/// Runs approximately once each frame and handles rendering.
	void update();
};
