#pragma once

#include <SFML/Graphics.hpp>

#include "Vars.h"
#include "Resources.h"
#include "Evaluator.h"
#include "Input.h"

#include "Console.h"
//#include "Background.h"
//#include "Sprites.h"
#include "Graphics.h"
#include "Panel.h"

struct Visual {
	const static int WIDTH = 256;
	const static int HEIGHT = 192;
	
	const static int PRIO_LEVELS = 4;
	const static int CON = 0;
	const static int BG = 1;
	const static int SP = 2;
	const static int GRP = 3;
	const static int ELEM_COUNT = 4;	
	
	Evaluator& e;
	Console c;
	//Background b;
	//Sprites s;
	Graphics g;
	Panel p;

	bool visible[6]; //CON, PNL, FG, BG, SP, GRP

	sf::Shader bgsp_shader;
	sf::Shader grp_shader;
	sf::Texture col_tex; //all colors fit pretty easily into one texture
	std::vector<sf::Texture> resource_tex;
	sf::Texture grp_tex;
	
	std::vector<std::vector<sf::Sprite>> display_sprites;
	sf::Texture display_texture;
	
	void visible_(const Args&);
	
public:
	Visual(Evaluator&, Resources&, Input&);

	std::map<Token, cmd_type> get_cmds();
	std::map<Token, op_func> get_funcs();
	
	void draw(sf::RenderWindow&);
};
