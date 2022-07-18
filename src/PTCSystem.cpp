#include "PTCSystem.hpp"
#include "Debugger.hpp"

#include <chrono>
#include <ctime>

void zoom(sf::Window& w, int scale){
	w.setSize(sf::Vector2u(256*scale, 384*scale));
}

PTCSystem::PTCSystem() {
	//Create objects for system
	resources = std::make_shared<Resources>();
	resources->load_default();
//	resources->load_program("resources/misc/TESTPKG.PTC"); //package program that contains every default resource
	
	evaluator = std::make_shared<Evaluator>();
	
	input = std::make_shared<Input>(*evaluator);
	
	visual = std::make_shared<Visual>(this);
	
	sound = std::make_shared<Sound>(this);
	
	evaluator->add_funcs(visual->get_funcs());
	evaluator->add_funcs(input->get_funcs());
	evaluator->add_funcs(sound->get_funcs());
	
	// load default controls
	{
		std::ifstream controls{"resources/config/controls.txt"};
		int code;
//		for (int b = 0; b < 4; ++b){
//			controls >> code;
//			special.push_back((sf::Keyboard::Key)code);
//		}
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
	
	get_resources()->load_program("resources/misc/LOADER.PTC");
	program = std::make_shared<Program>(this);
	
	program->add_cmds(visual->get_cmds());
	program->add_cmds(input->get_cmds());
	program->add_cmds(sound->get_cmds());
}

void PTCSystem::load_program(std::string filename){
	resources->load_program(filename);
	program->set_tokens(tokenize(resources->prg));
	visual->regen_all();
}

void PTCSystem::set_option(std::string option, int state){
	if (option == "-s"){
		sound->_enable(static_cast<bool>(state));
	}
	if (option == "-a"){
		program->_reload(state);
	}
}

void PTCSystem::update(){
	get_visual()->update(); //update frame etc.
}

void PTCSystemDisplay::set_option(std::string option, int state){
	PTCSystem::set_option(option, state);
	if (option == "-d"){
		debug = std::make_unique<Debugger>(this);
	}
}

PTCSystemDisplay::PTCSystemDisplay() : PTCSystem(), window{sf::VideoMode(256, 384), "PTCEmukII"} {
	// https://en.sfml-dev.org/forums/index.php?topic=20033.0 ????
	zoom(window, 2);
	window.setFramerateLimit(60);
}

void PTCSystemDisplay::update(){
	PTCSystem::update();
	//Check SFML events
	sf::Event event;
	sf::Keyboard::Key k = sf::Keyboard::Key::Unknown;
	int unicode = 0;
	while (window.isOpen() && window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed){
			window.close();
			return; //nothing more to be done
		}
		if (event.type == sf::Event::KeyPressed){
			k = event.key.code;
		}
		if (event.type == sf::Event::TextEntered){
			unicode = event.text.unicode;
			
//			std::cout << unicode << std::endl;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt)){
				if (unicode < 0x80) //modify characters in ASCII range
					unicode |= 0x80;
			}
		}
	}
	
	//hardcoded special buttons
	if (k == sf::Keyboard::Key::F1){
		zoom(window, 1);
	} else if (k == sf::Keyboard::Key::F2){
		zoom(window, 2);
	} else if (k == sf::Keyboard::Key::F5){
		get_program()->restart();
	} else if (k == sf::Keyboard::Key::F10){
		keybutton_enable = !keybutton_enable;
	} else if (k == sf::Keyboard::Key::F11){
		keyboard_enable = !keyboard_enable;
	} else if (k == sf::Keyboard::Key::F12){
		debug = std::make_unique<Debugger>(this);
	}
	
	int b = 0;
	//controller buttons are always enabled
	for (const auto jkp : get_input()->joy_to_button){
		if (sf::Joystick::isButtonPressed(0, jkp.first)){
			b|=jkp.second;
		}
	}
	//controller sticks
	for (auto jxp : get_input()->stick_to_button){
		auto pos = sf::Joystick::getAxisPosition(0, (sf::Joystick::Axis)jxp.first);
		if (pos > get_input()->sensitivity)
			b|=2*jxp.second;
		if (pos < -get_input()->sensitivity)
			b|=jxp.second;
	}
	//only use keyboard for buttons if enabled
	if (keybutton_enable){
		for (const auto kp : get_input()->code_to_button){
			if (sf::Keyboard::isKeyPressed(kp.first)){
				b|=kp.second;
			}
		}
	}
	
	get_input()->update(b);
	
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
		get_input()->touch(mouse_press, mouse_x, mouse_y);
		
		int old_keyboard = get_input()->keyboard;
		get_visual()->p.touch_keys(mouse_time>0, mouse_x, mouse_y);
		get_input()->touch_key(get_visual()->p.get_last_keycode());
		
		if (get_input()->keyboard != old_keyboard){
			get_resources()->load_keyboard(get_input()->keyboard);
			get_visual()->regen_chr("SPK2");
			get_visual()->regen_chr("SPK3");
		}
		
	} else if (keyboard_enable && unicode && get_visual()->p.panel_on() > 1){
		// keyboard write directly to inkey buffer (no screen touch)
		get_input()->type(unicode, k);
	} else {
		get_visual()->p.touch_keys(false, -1, -1);
		get_input()->touch(mouse_press, mouse_x, mouse_y);
	}
	
	//Draw current state
	window.clear();
	
	get_visual()->draw(window);
	
	window.display();
	
	if (debug){
		debug->update();
	}
}
