#pragma once

#include <SFML/Graphics.hpp>

#include "Evaluator.hpp"
#include "Resources.hpp"
#include "Program.hpp"
#include "Visual.hpp"
#include "Input.hpp"
#include "Sound.hpp"
#include "Debugger.hpp"
//#include "InstructionTimer.hpp"

#include <memory>
#include <fstream>

/// System class that manages all components of the PTC system.
class PTCSystem {
	//Variety of components needed for the system to work
	/// Evaluator object
	std::shared_ptr<Evaluator> evaluator;
	/// Resources object. Handles resource management
	std::shared_ptr<Resources> resources;
	/// Program object. Handles the actual program execution
	std::shared_ptr<Program> program;
	/// Visual object. Responsible for rendering and any visual components.
	std::shared_ptr<Visual> visual;
	/// Input object. Takes input from this object and manages user input of programs
	std::shared_ptr<Input> input;
	/// Sound object. Manages the sound system.
	std::shared_ptr<Sound> sound;
	// Timer object to approximate execution timing
	// InstructionTimer timer;
	
public:
	/// Constructor. Initializes all of the important subcomponent objects.
	PTCSystem();
	
	/// Constructor. Initializes all of the important subcomponent objects.
	PTCSystem(std::string);
	
	/// Copy constructor (deleted)
	PTCSystem(const PTCSystem&) = delete;
	
	/// Copy assignment (deleted)
	PTCSystem& operator=(const PTCSystem&) = delete;
	
	/// Destructor
	virtual ~PTCSystem(){}
	
	/// Sets the state of certain objects based on an option string.
	/// 
	/// @param option Option string
	/// @param state State of object
	virtual void set_option(std::string option, int state);
	
	/// Loads a new program, updating all resources required.
	///
	/// @param filename Filename of program to load
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
	
	/// Gets the resources object of this system.
	/// 
	/// @return Resources object
	std::shared_ptr<Resources> get_resources(){
		return resources;
	}
	
	/// Gets the evaluator object of this system.
	/// 
	/// @return Evaluator object
	std::shared_ptr<Evaluator> get_evaluator(){
		return evaluator;
	}
	
	/// Gets the program object of this system.
	/// 
	/// @return Program object
	std::shared_ptr<Program> get_program(){
		return program;
	}
	
	/// Gets the visual object of this system.
	/// 
	/// @return Visual object
	std::shared_ptr<Visual> get_visual(){
		return visual;
	}
	
	/// Gets the input object of this system.
	/// 
	/// @return Input object
	std::shared_ptr<Input> get_input(){
		return input;
	}
	
	/// Gets the sound object of this system.
	/// 
	/// @return Sound object
	std::shared_ptr<Sound> get_sound(){
		return sound;
	}
	
	/// Run each frame to update time based events.
	void update();
};

/// Class that creates and displays a PTCSystem
class PTCSystemDisplay : public PTCSystem {
	/// Debugger object
	std::unique_ptr<Debugger> debug;
	
	/// Display window
	sf::RenderWindow window;
	
	//Window and keyboard control
//	std::vector<sf::Keyboard::Key> special{};
	/// Use keyboard as controller/button input
	bool keybutton_enable = true; // Use keyboard as controller
	/// Use keyboard as keyboard (type to enter letters)
	bool keyboard_enable = true; // Use keyboard to simulate touch keyboard

	//touch control
	/// Is the mouse currently being clicked?
	bool mouse_press = false;
	/// How long the mouse was held down
	int mouse_time = 0;
	/// Last mouse x coordinate
	int mouse_x = 0;
	/// Last mouse y coordinate
	int mouse_y = 0;
	
public:
	/// Default constructor
	PTCSystemDisplay(std::string program);
	
	/// Copy constructor (deleted)
	PTCSystemDisplay(const PTCSystemDisplay&) = delete;
	
	/// Copy assignment (deleted)
	PTCSystemDisplay& operator=(const PTCSystemDisplay&) = delete;
	
	void set_option(std::string option, int state) override;
	
	/// Check if the system is currently running (Is the window still open)
	/// 
	/// @return Is system currently running?
	bool is_ok(){
		return window.isOpen();
	}
	
	/// Runs approximately once each frame and handles rendering.
	void update();
};
