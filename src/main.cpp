#include <SFML/Graphics.hpp>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"
#include "Resources.h"
#include "Program.h"
#include "Visual.h"
#include "Input.h"

#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(256, 384), "PTCEmukII");
    
	if (!sf::Shader::isAvailable()){
        std::cout << "Error: Shaders are unavailable";
        return 0;
    }

    sf::Shader s;
    if (!s.loadFromFile("resources/shaders/bgsp.frag", sf::Shader::Fragment)){
        std::cout << "Error: Shader could not be loaded";
        return 0;
    }
    
	Evaluator e{};
	
	Resources r{};
	r.load_program("programs/SAMPLE2.PTC");
	r.load_default();
	
	auto tk = tokenize(r.prg);
	
	Program program(e, tk);
	Visual v{e, r};
	Input i{e};
	//Console console(e, r.chr.at("BGF0U"));
	
	e.add_funcs(v.get_funcs());
	e.add_funcs(i.get_funcs());
	program.add_cmds(v.get_cmds());
	program.add_cmds(i.get_cmds());
	
	/*int n = 0;
	while(!program.at_eof()){
		auto tok = program.next_instruction();
		auto proc = program.split(tok);
		for (uint32_t p = 0; p < proc.size(); ++p){ 
			print("V"+std::to_string(p), proc[p]);
		}
		
		n++;
		print("Instr " + std::to_string(n), tok);
	}*/
		
	//print("TOKENIZED:", tk);
	sf::Texture contx{};
	contx.create(256, 192);
	sf::Sprite consp{};
	consp.setTexture(contx);

	sf::Texture color_tex{}; 
	color_tex.create(256, 1);
	color_tex.update(r.col.at("COL0U").COL_to_RGBA().data());
	
	program.run();
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //get updated textures for drawing to screen
        
        window.clear();
	
		s.setUniform("texture", sf::Shader::CurrentTexture);
    	s.setUniform("colors", color_tex);
	
		contx.update(v.c.draw().data());
		window.draw(consp, &s);
        //draw textures

        window.display();
    }

    return 0;
}
