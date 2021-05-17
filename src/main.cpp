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
#include <chrono>
#include <ctime>

int main()
{
    
	Evaluator e{};
	
	std::string prgname;
	std::cout << "Enter a program name, file must be in 'programs/':" << std::endl;
	std::cout << "Example: SAMPLE1.PTC" << std::endl;
	std::cin >> prgname;

    sf::RenderWindow window(sf::VideoMode(256, 384), "PTCEmukII");
	window.setVerticalSyncEnabled(true);
	
	Resources r{};
	r.load_program("programs/" + prgname);
	r.load_default();
	
	auto tk = tokenize(r.prg);
	
	Program program(e, tk);
	Input i{e};
	Visual v{e, r, i};
	Sound s{e};
	//Console console(e, r.chr.at("BGF0U"));
	
	e.add_funcs(v.get_funcs());
	e.add_funcs(i.get_funcs());
	e.add_funcs(s.get_funcs());
	program.add_cmds(v.get_cmds());
	program.add_cmds(i.get_cmds());
	program.add_cmds(s.get_cmds());
			
	//print("TOKENIZED:", tk);	
	program.run();
	
	bool mouse_press = false;
	int mouse_x = 0;
	int mouse_y = 0;
    while (window.isOpen())
    {
		sf::Event event;

    	sf::Keyboard::Key k = sf::Keyboard::Key::Unknown;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){
				window.close();
				break;
			}
			if (event.type == sf::Event::KeyPressed){
				k = event.key.code;
			}
			if (event.type == sf::Event::MouseButtonPressed){
				mouse_x = event.mouseButton.x;
				mouse_y = event.mouseButton.y;
				mouse_press = true;
			}
			if (event.type == sf::Event::MouseButtonReleased){
				mouse_press = false;
			}
		}
		if (!window.isOpen())
			break;
		//update buttons
		int b = 0;
		for (const auto kp : i.code_to_button){
			if (sf::Keyboard::isKeyPressed(kp.first)){
				b+=kp.second;
			}
		}
		
		i.update(b, k);
		i.touch(mouse_press, mouse_x, mouse_y);
		
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
		strftime(date, 11, "%F", tm);
		strftime(time, 9, "%T", tm);
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
