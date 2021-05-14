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

int main()
{
    
	Evaluator e{};
	
	std::string prgname;
	std::cout << "Enter a program name, file must be in 'programs/':" << std::endl;
	std::cout << "Example: SAMPLE1.PTC" << std::endl;
	std::cin >> prgname;

    sf::RenderWindow window(sf::VideoMode(256, 384), "PTCEmukII");
	
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
		//update buttons
		int b = 0;
		for (const auto kp : i.code_to_button){
			if (sf::Keyboard::isKeyPressed(kp.first)){
				b+=kp.second;
			}
		}
		
		i.update(b, k);
		i.touch(mouse_press, mouse_x, mouse_y);
		
        //get updated textures for drawing to screen
        window.clear();
	
		v.draw(window);
	
		window.display();
	}

	return 0;
}
