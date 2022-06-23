#include "PTCSystem.hpp"

#include <chrono>
#include <ctime>

void zoom(sf::Window& w, int scale){
	w.setSize(sf::Vector2u(256*scale, 384*scale));
}

PTCSystem::PTCSystem() : window{sf::VideoMode(256, 384), "PTCEmukII"} {
	// https://en.sfml-dev.org/forums/index.php?topic=20033.0 ????
	zoom(window, 2);
	window.setFramerateLimit(60);

	//Create objects for system
	resources = std::make_shared<Resources>();
	resources->load_default();
	
	evaluator = std::make_shared<Evaluator>();
	
	input = std::make_shared<Input>(*evaluator);
	
	visual = std::make_shared<Visual>(*evaluator, *resources, *input);
	
	sound = std::make_shared<Sound>(*evaluator);
	
	evaluator->add_funcs(visual->get_funcs());
	evaluator->add_funcs(input->get_funcs());
	evaluator->add_funcs(sound->get_funcs());
	
	// load default controls
	{
		std::ifstream controls{"config/controls.txt"};
		int code;
		for (int b = 0; b < 4; ++b){
			controls >> code;
			special.push_back((sf::Keyboard::Key)code);
		}
		for (int b = 0; b < 12; ++b){
			controls >> code;
			input->code_to_button.insert(std::pair((sf::Keyboard::Key)code, 1 << b));
		}
		controls >> code;
		input->sensitivity = code;
		for (int s = 0; s < 2; s++){
			controls >> code;
			input->stick_to_button.insert(std::pair(code, 1 << (s*2)));
		}
		for (int b = 4; b < 12; ++b){
			controls >> code;
			input->joy_to_button.insert(std::pair(code, 1 << b));
		}
	}
	
	PRG prg;
	prg.load("resources/misc/LOADER.PTC");
	program = std::make_shared<Program>(*evaluator, tokenize(prg));
	
	program->add_cmds(visual->get_cmds());
	program->add_cmds(input->get_cmds());
	program->add_cmds(sound->get_cmds());
}

void PTCSystem::set_option(std::string option, int state){
	if (option == "-s"){
		sound->_enable(static_cast<bool>(state));
	}
}

void PTCSystem::update(){
	//Check SFML events
	sf::Event event;
	sf::Keyboard::Key k = sf::Keyboard::Key::Unknown;
	while (window.isOpen() && window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed){
			window.close();
			return; //nothing more to be done
		}
		if (event.type == sf::Event::KeyPressed){
			if (keyboard_enable)
				k = event.key.code;
		}
	}
	
	//special buttons
	if (sf::Keyboard::isKeyPressed(special[0])){
		zoom(window, 1);
	}
	if (sf::Keyboard::isKeyPressed(special[1])){
		zoom(window, 2);
	}
	if (sf::Keyboard::isKeyPressed(special[2])){
		keybutton_enable = !keybutton_enable;
	}
	if (sf::Keyboard::isKeyPressed(special[3])){
		keyboard_enable = !keyboard_enable;
	}
	
	int b = 0;
	//controller buttons are always enabled
	for (const auto jkp : input->joy_to_button){
		if (sf::Joystick::isButtonPressed(0, jkp.first)){
			b|=jkp.second;
		}
	}
	//controller sticks
	for (auto jxp : input->stick_to_button){
		auto pos = sf::Joystick::getAxisPosition(0, (sf::Joystick::Axis)jxp.first);
		if (pos > input->sensitivity)
			b|=2*jxp.second;
		if (pos < -input->sensitivity)
			b|=jxp.second;
	}
	//only use keyboard for buttons if enabled
	if (keybutton_enable){
		for (const auto kp : input->code_to_button){
			if (sf::Keyboard::isKeyPressed(kp.first)){
				b|=kp.second;
			}
		}
	}
	
	input->update(b);
	
	//Update mouse info/Touchscreen info
	mouse_press = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	if (mouse_press){
		sf::Vector2u size = window.getSize();
		double w = size.x;
		double h = size.y;
		
		auto [x, y] = sf::Mouse::getPosition(window);
		mouse_x = 256 * x / w;
		mouse_y = 192 * y / (h/2) - 192;
		++mouse_time;
	} else {
		mouse_time = 0;
	}
	
	if (mouse_press){
		//"touchscreen" input
		input->touch(mouse_press, mouse_x, mouse_y);
		visual->p.touch_keys(mouse_time==1, mouse_x, mouse_y);
		input->touch_key(visual->p.get_last_keycode());
	} else if (k != sf::Keyboard::Key::Unknown){
		//Simluate touchscreen tap by physical keyboard presses
		int keycode = input->keyboard_to_keycode(k);
		if (keycode != 0){
			auto [x,y] = visual->p.get_keycode_xy(keycode);
			
			input->touch(true, x, y);
			visual->p.touch_keys(true, x, y);
			input->touch_key(keycode);
		}
	} else {
		visual->p.touch_keys(false, -1, -1);
		input->touch(mouse_press, mouse_x, mouse_y);
	}
	
	//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
	//https://en.cppreference.com/w/cpp/chrono
	auto time_t = std::time(0);
	std::tm* tm = std::localtime(&time_t);
	char date[10+1];//"yyyy/mm/dd\0";
	char time[8+1]; //"hh:mm:ss\0";
	//date[10]='\0';
	//time[8]='\0';
	strftime(date, 11, "%Y/%m/%d", tm);
	strftime(time, 9, "%H:%M:%S", tm);
	date[4] = '/';
	date[7] = '/';
	
	//Sets some system variables
	evaluator->vars.write_sysvar("DATE$", std::string(date));
	evaluator->vars.write_sysvar("TIME$", std::string(time));

	//Draw current state
	window.clear();
	
	visual->draw(window);
	visual->update(); //update frame etc.
	
	window.display();
}
