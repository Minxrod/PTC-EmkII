#include <SFML/Graphics.hpp>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"
#include "Resources.h"
#include "Program.h"
#include "Visual.h"
#include "Input.h"
#include "Sound.h"
#include "Variables.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <map>

void zoom(sf::Window& w, int scale){
	w.setSize(sf::Vector2u(256*scale, 384*scale));
}

int main(){	
	Evaluator e{};
	
	std::vector<sf::Keyboard::Key> special{};
	
	Input i{e};
	{
		std::ifstream controls{"config/controls.txt"};
		int code;
		for (int b = 0; b < 4; ++b){
			controls >> code;
			special.push_back((sf::Keyboard::Key)code);
		}
		for (int b = 0; b < 12; ++b){
			controls >> code;
			i.code_to_button.insert(std::pair((sf::Keyboard::Key)code, 1 << b));
		}
		controls >> code;
		i.sensitivity = code;
		for (int s = 0; s < 2; s++){
			controls >> code;
			i.stick_to_button.insert(std::pair(code, 1 << (s*2)));
		}
		for (int b = 4; b < 12; ++b){
			controls >> code;
			i.joy_to_button.insert(std::pair(code, 1 << b));
		}
	}
	
	std::string prgname;
	std::cout << "Enter a program name, file must be in 'programs/':" << std::endl;
	std::cout << "Example: ENG1.PTC" << std::endl;
	std::cin >> prgname;

    sf::RenderWindow window(sf::VideoMode(256, 384), "PTCEmukII");
	window.setVerticalSyncEnabled(true);
	//window.setJoystickThreshold(25.f);
	
	Resources r{};
	r.load_program("programs/" + prgname);
	r.load_default();
	
	auto tk = tokenize(r.prg);
	
	Program program(e, tk);
	Visual v{e, r, i};
	Sound s{e};
	//Console console(e, r.chr.at("BGF0U"));
	
	e.add_funcs(v.get_funcs());
	e.add_funcs(i.get_funcs());
	e.add_funcs(s.get_funcs());
	program.add_cmds(v.get_cmds());
	program.add_cmds(i.get_cmds());
	program.add_cmds(s.get_cmds());
			
	print("TOKENIZED:", tk);	
	program.run();
	
	bool keybutton_enable = true;
	bool keyboard_enable = true;
	bool mouse_press = false;
	int mouse_time = 0;
	int mouse_x = 0;
	int mouse_y = 0;
    while (window.isOpen())
    {
		sf::Event event;

    	sf::Keyboard::Key k = sf::Keyboard::Key::Unknown;
		while (window.isOpen() && window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();	
				break;
			}
			if (event.type == sf::Event::KeyPressed){
				if (keyboard_enable)
					k = event.key.code;
			}
			if (event.type == sf::Event::MouseButtonPressed){
				mouse_press = true;
			}
			if (event.type == sf::Event::MouseButtonReleased){
				mouse_press = false;
			}
		}
		//Update mouse info
		if (mouse_press){
			auto [x, y] = sf::Mouse::getPosition(window);
			mouse_x = x;
			mouse_y = y - 192;
			++mouse_time;
		} else {
			mouse_time = 0;
		}
		
		if (!window.isOpen())
			break;
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
			
		//update buttons
		int b = 0;
		//controller buttons are always enabled
		for (const auto jkp : i.joy_to_button){
			if (sf::Joystick::isButtonPressed(0, jkp.first)){
				b|=jkp.second;
			}
		}
		//controller sticks
		for (auto jxp : i.stick_to_button){
			auto pos = sf::Joystick::getAxisPosition(0, (sf::Joystick::Axis)jxp.first);
			if (pos > i.sensitivity)
				b|=2*jxp.second;
			if (pos < -i.sensitivity)
				b|=jxp.second;
		}
		//only use if turned on
		if (keybutton_enable){
			for (const auto kp : i.code_to_button){
				if (sf::Keyboard::isKeyPressed(kp.first)){
					b|=kp.second;
				}
			}
		}
		
		i.update(b); //only buttons now
		if (mouse_press){
			//"touchscreen" input
			i.touch(mouse_press, mouse_x, mouse_y);
			v.p.touch_keys(mouse_time==1, mouse_x, mouse_y);
			i.touch_key(v.p.get_last_keycode());
		} else if (k != sf::Keyboard::Key::Unknown){
			//Simluate touchscreen tap by physical keyboard presses
			int keycode = i.keyboard_to_keycode(k);
			if (keycode != 0){
				auto [x,y] = v.p.get_keycode_xy(keycode);
				
				i.touch(true, x, y);
				v.p.touch_keys(true, x, y);
				i.touch_key(keycode);
			}
		} else {
			v.p.touch_keys(false, -1, -1);
		}
		
		//begin chunk to be moved
		//todo: move to somewhere else (likely Program)
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
		
		e.vars.write_sysvar("DATE$", std::string(date));
		e.vars.write_sysvar("TIME$", std::string(time));
		//end chunk to be moved
		
        //get updated textures for drawing to screen
        window.clear();
	
		v.draw(window);
		v.update();
	
		window.display();
	}
	


	return 0;
}
