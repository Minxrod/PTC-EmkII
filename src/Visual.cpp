#include "Visual.h"
#include "Vars.h"

Visual::Visual(Evaluator& ev, Resources& rs, Input& i) :
	e{ev},
	r{rs},
	c{ev, r.chr.at("BGF0U"), i},
	b{ev, r.scr},
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
	}
	maincntl = std::get<Number*>(e.vars.get_var_ptr("MAINCNTL"));
	maincnth = std::get<Number*>(e.vars.get_var_ptr("MAINCNTH"));
	//creates a color texture from all colors COL0U to COL2L
	col_tex.create(256,6);
	regen_col();
	//create all CHR resource textures
	resource_tex = std::vector<sf::Texture>{12, sf::Texture{}};
	
	resource_tex[0].create(256, 64);
	resource_tex[0].update(r.chr.at("BGF0U").get_array().data());
	
	resource_tex[1].create(256, 64*4);
	for (int i = 0; i <= 3; ++i)
		resource_tex[1].update(r.chr.at("BGU"+std::to_string(i)+"U").get_array().data(), 256, 64, 0, i*64);
	
	grp_tex.create(256,192);
}

void Visual::regen_col(){
	for (int i = 0; i < 6; ++i){
		std::string cr = "COL" + std::to_string(i%3) + (i/3>0 ? "L" : "U");
		col_tex.update(r.col.at(cr).COL_to_RGBA().data(), WIDTH, 1, 0, i);
	}
}

std::map<Token, cmd_type> Visual::get_cmds(){
	auto cmds = std::map<Token, cmd_type>{
		cmd_map{"VISIBLE"_TC, getfunc(this, &Visual::visible_)},
		cmd_map{"VSYNC"_TC, getfunc(this, &Visual::vsync_)},
		cmd_map("CHRINIT"_TC, getfunc(this, &Visual::chrinit_)),
//		cmd_map("CHRSET"_TC, getfunc(this, &Visual::chrset_)),
//		cmd_map("CHRREAD"_TC, getfunc(this, &Visual::chrread_)),
		cmd_map("COLINIT"_TC, getfunc(this, &Visual::colinit_)),
//		cmd_map("COLSET"_TC, getfunc(this, &Visual::colset_)),
//		cmd_map("COLREAD"_TC, getfunc(this, &Visual::colread_)),
		cmd_map("ACLS"_TC, getfunc(this, &Visual::acls_)),
	};
	
	cmds.merge(c.get_cmds());
	cmds.merge(b.get_cmds());
	cmds.merge(g.get_cmds());
	cmds.merge(p.get_cmds());
	return cmds;
}

std::map<Token, op_func> Visual::get_funcs(){
	auto funcs = c.get_funcs();
	funcs.merge(b.get_funcs());
	funcs.merge(g.get_funcs());
	funcs.merge(p.get_funcs());
	return funcs;
}

void Visual::visible_(const Args& a){
	for (int i = 0; i < 6; i++)
		visible[i] = (bool)(int)std::get<Number>(e.evaluate(a[i+1]));
}

void Visual::vsync_(const Args& a){
	int frames = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int wait_until = (int)*maincntl + frames;
	
	while (*maincntl < wait_until){
		//?? idk;
	}
}

void Visual::chrinit_(const Args& a){
	//CHRINIT resource
	String res = std::get<String>(e.evaluate(a[1]));
	if (res.size() == 3){
		res += "0"; //CHRINIT "BGU" initializes BGU0 apparently
	}
	if (res.size() == 4){
		res += "U"; //default resources are upper screen
	}
	std::string path = "resources/graphics/"+res.substr(0,4)+".PTC";
	load_default(r.chr.at(res), path);
	//needs to regen textures
}

void Visual::chrset_(const Args&){
	
}

void Visual::chrread_(const Args&){
	
}

void Visual::colinit_(const Args& a){
	//COLINIT resource color
	//BG, SP, GRP
	//Affects both upper and lower screens
	String res = std::get<String>(e.evaluate(a[1]));
	if (res == "BG"){res = "COL0";}
	if (res == "SP"){res = "COL1";}
	if (res == "GRP"){res = "COL2";}
	
	std::string path = "resources/graphics/"+res.substr(0,4)+".PTC";
	if (a.size() == 2){
		load_default(r.col.at(res+"U"), path);
		load_default(r.col.at(res+"L"), path);
	} else {
		//needs to write specific color
		//(not currently stored, needs to be done differently)
	}
}

void Visual::colset_(const Args&){
	
}

void Visual::colread_(const Args&){
	
}

void Visual::acls_(const Args&){
	//VISIBLE 1,1,1,1,1,1
	for (int i = 0; i < 6; i++)
		visible[i] = true;
	//ICONCLR
	//COLOR 0:CLS
	//GDRAWMD FALSE
}

void Visual::update(){
	e.vars.write_sysvar("MAINCNTL", *maincntl+1);
	//do interpolation here as well (bg, sp)
}

//note: don't make this silly mistake
//https://en.sfml-dev.org/forums/index.php?topic=13526.0
void Visual::draw(sf::RenderWindow& w){
	//common
	bgsp_shader.setUniform("colors", col_tex);
	bgsp_shader.setUniform("texture", sf::Shader::CurrentTexture);
	sf::RenderStates s;
	s.shader = &bgsp_shader;
	//grp
	sf::Sprite grp;
	grp_tex.update(g.draw().data());
	grp.setTexture(grp_tex);
	grp.setColor(sf::Color(0));
	bgsp_shader.setUniform("colbank", 2.0f);
	w.draw(grp, s);
	//bg
	bgsp_shader.setUniform("colbank", 0.0f);
	s.texture = &resource_tex[1];
	for (int l = 1; l >= 0; --l){
		auto& bg = b.draw(0, l);
		auto pos = bg.getPosition();
		if (pos.x > 0){
			bg.setPosition(pos.x - 64*8, pos.y);
			w.draw(bg, s);
		}
		if (pos.y > 0){
			bg.setPosition(pos.x, pos.y - 64*8);
			w.draw(bg, s);
		}
		if (pos.x > 0 && pos.y > 0){
			bg.setPosition(pos.x - 64*8, pos.y - 64*8);
			w.draw(bg, s);
		}
		bg.setPosition(pos);
		w.draw(bg, s);
	}	
	//console
	s.texture = &resource_tex[0];
	auto& con = c.draw();
	bgsp_shader.setUniform("colbank", 0.0f);
	w.draw(con, s);
	
}
