#include "Visual.h"
#include "Vars.h"

Visual::Visual(Evaluator& ev, Resources& r, Input& i) :
	e{ev},
	c{ev, r.chr.at("BGF0U"), i},
	g{ev, r.grp},
	p{ev, r, i},
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
	//creates a color texture from all colors COL0U to COL2L
	col_tex.create(256,6);
	for (int i = 0; i < 6; ++i){
		std::string cr = "COL" + std::to_string(i%3) + (i/3>0 ? "L" : "U");
		col_tex.update(r.col.at(cr).COL_to_RGBA().data(), WIDTH, 1, 0, i);
	}
	//create all CHR resource textures
	resource_tex = std::vector<sf::Texture>{12, sf::Texture{}};
	resource_tex[0].create(256, 64);
	resource_tex[0].update(r.chr.at("BGF0U").get_array().data());
}

std::map<Token, cmd_type> Visual::get_cmds(){
	auto cmds = std::map<Token, cmd_type>{
		cmd_map{"VISIBLE"_TC, getfunc(this, &Visual::visible_)},
	};
	
	cmds.merge(c.get_cmds());
	cmds.merge(g.get_cmds());
	cmds.merge(p.get_cmds());
	return cmds;
}

std::map<Token, op_func> Visual::get_funcs(){
	auto funcs = c.get_funcs();
	funcs.merge(g.get_funcs());
	funcs.merge(p.get_funcs());
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
	bgsp_shader.setUniform("colbank", 0.0f);
	
	//display_texture.update(c.draw().data(), WIDTH, HEIGHT, 0*WIDTH, CON*HEIGHT);
	//display_texture.update(g.draw().data(), WIDTH, HEIGHT, 0*WIDTH, GRP*HEIGHT);
	sf::RenderStates s;
	s.shader = &bgsp_shader;
	s.texture = &resource_tex[0];
	auto& t = c.draw();
	w.draw(t, s);
	
	/*for (int prio = 3; prio >= 0; --prio){
		auto& prio_sprites = display_sprites[prio];
		for (int elem = 3; elem >= 0; --elem){
			auto& ds = prio_sprites[elem];
			w.draw(ds, &bgsp_shader);
		}
	}*/
}
