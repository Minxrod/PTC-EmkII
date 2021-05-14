#include "Visual.h"
#include "Vars.h"

Visual::Visual(Evaluator& ev, Resources& r, Input& i) :
	e{ev},
	c{ev, r.chr.at("BGF0U"), i},
	g{ev, r.grp},
	visible{true,true,true,true,true,true}
{
	if (!sf::Shader::isAvailable()){
		std::cout << "Error: Shaders are unavailable";
	} else {
		if (!bgsp_shader.loadFromFile("resources/shaders/bgsp.frag", sf::Shader::Fragment)){
			std::cout << "Error: Shader could not be loaded";
		}
		if (!grp_shader.loadFromFile("resources/shaders/grp.frag", sf::Shader::Fragment)){
			std::cout << "Error: Shader could not be loaded";
		}
	}
	//each sprite is the size of one screen, and will handle a combination
	//of priority level and graphical element.
	display_texture.create(WIDTH * PRIO_LEVELS, HEIGHT * ELEM_COUNT);
	display_sprites = std::vector<std::vector<sf::Sprite>>(PRIO_LEVELS, std::vector<sf::Sprite>(ELEM_COUNT, sf::Sprite{}));
	for (int p = 0; p < PRIO_LEVELS; ++p){
		for (int q = 0; q < ELEM_COUNT; ++q){
			display_sprites[p][q].setTexture(display_texture);
			display_sprites[p][q].setTextureRect(sf::IntRect(p*WIDTH,q*HEIGHT,WIDTH,HEIGHT));
		}
	}
	//creates a color texture from all colors COL0U to COL2L
	col_tex.create(256,6);
	for (int i = 0; i < 6; ++i){
		std::string cr = "COL" + std::to_string(i%3) + (i/3>0 ? "L" : "U");
		col_tex.update(r.col.at(cr).COL_to_RGBA().data(), WIDTH, 1, 0, i);
	}
}

std::map<Token, cmd_type> Visual::get_cmds(){
	auto cmds = std::map<Token, cmd_type>{
		cmd_map{"VISIBLE"_TC, getfunc(this, &Visual::visible_)},
	};
	
	cmds.merge(c.get_cmds());
	cmds.merge(g.get_cmds());
	return cmds;
}

std::map<Token, op_func> Visual::get_funcs(){
	auto funcs = c.get_funcs();
	funcs.merge(g.get_funcs());
	return funcs;
}

void Visual::visible_(const Args& a){
	for (int i = 0; i < 6; i++)
		visible[i] = (bool)(int)std::get<Number>(e.evaluate(a[i+1]));
}

//note: don't make this silly mistake
//https://en.sfml-dev.org/forums/index.php?topic=13526.0
void Visual::draw(sf::RenderWindow& w){
	bgsp_shader.setUniform("colors", col_tex);
	bgsp_shader.setUniform("texture", sf::Shader::CurrentTexture);
	bgsp_shader.setUniform("palette", 0.0f);
	bgsp_shader.setUniform("colbank", 0.0f);	

	auto res = c.draw();
	
	display_texture.update(res.data(), 256, 192, 0, 0);
		
	for (auto& prio_sprites : display_sprites){
		for (auto& ds : prio_sprites){
			w.draw(ds, &bgsp_shader);
		}
	}
}
