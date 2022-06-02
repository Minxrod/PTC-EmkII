#include "Visual.h"
#include "Vars.h"

const float COL_BG = 0.1;
const float COL_SP = 1.1;
const float COL_GRP = 2.1;

Visual::Visual(Evaluator& ev, Resources& rs, Input& i) :
	e{ev},
	r{rs},
	c{ev, r.chr.at("BGF0U"), i},
	b{ev, r.scr},
	s{ev},
	g{ev, r.grp, r, this},
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
	std::vector<std::string> type{"BGF", "BGU", "SPU", "SPS", "SPD", "BGD"};
	std::vector<int> banks{1, 4, 8, 2, 4, 2};
	
	resource_tex = std::vector<sf::Texture>{12, sf::Texture{}};
	
	for (int i = 0; i < (int)resource_tex.size(); ++i){
		resource_tex[i].create(256, 64*(i != 10 ? banks[i%6] : 8));
		if (i == 8 || i == 4){
			continue;
		}
		std::string screen = ((i>=6)?"L":"U");
		if (i == 2 || i == 10){
			screen = "";
		}
		
		for (int b = 0; b < banks[i%6]; ++b)
			resource_tex[i].update(r.chr.at(type[i%6]+std::to_string(b)+screen).get_array().data(), 256, 64, 0, b*64);
	}
	//append SPK to SPD
	for (int b = 4; b < 8; ++b)
		resource_tex[10].update(r.chr.at("SPK"+std::to_string(b-4)).get_array().data(), 256, 64, 0, b*64);
	
	grp_tex.create(256,192);
}

void Visual::regen_chr(std::string type){
	int chr_bank = type[3] - '0';
	std::vector<std::string> types{"BGF", "BGU", "SPU", "SPS", "SPD", "BGD"};
	int tex_num = std::distance(types.begin(), std::find(types.begin(), types.end(), type.substr(0,3)));
	int screen = (type.size() > 4 && type[4]=='L') ? 6 : 0;
	resource_tex[tex_num + screen].update(r.chr.at(type).get_array().data(), 256, 64, 0, chr_bank*64);
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
		cmd_map("CHRSET"_TC, getfunc(this, &Visual::chrset_)),
		cmd_map("CHRREAD"_TC, getfunc(this, &Visual::chrread_)),
		cmd_map("COLINIT"_TC, getfunc(this, &Visual::colinit_)),
//		cmd_map("COLSET"_TC, getfunc(this, &Visual::colset_)),
//		cmd_map("COLREAD"_TC, getfunc(this, &Visual::colread_)),
		cmd_map("ACLS"_TC, getfunc(this, &Visual::acls_)),
		cmd_map("CLS"_TC, getfunc(this, &Visual::cls_)),
		cmd_map("LOAD"_TC, getfunc(this, &Visual::load_)),
		cmd_map("SAVE"_TC, getfunc(this, &Visual::save_)),
	};
	
	cmds.merge(c.get_cmds());
	cmds.merge(b.get_cmds());
	cmds.merge(s.get_cmds());
	cmds.merge(g.get_cmds());
	cmds.merge(p.get_cmds());
	return cmds;
}

std::map<Token, op_func> Visual::get_funcs(){
	auto funcs = c.get_funcs();
	funcs.merge(b.get_funcs());
	funcs.merge(s.get_funcs());
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
		if (res.substr(0,3) != "SPU" && res.substr(0,3) != "SPD")
			res += "U"; //default resources are upper screen
	}
	std::string path = "resources/graphics/"+res.substr(0,4)+".PTC";
	load_default(r.chr.at(res), path);
	//needs to regen textures
	regen_chr(res);
}

void Visual::chrset_(const Args& a){
	auto resource_type = std::get<String>(e.evaluate(a[1]));
	int chr_id = std::get<Number>(e.evaluate(a[2]));
	auto& chr_resource = r.chr.at(resource_type);
	std::string chr_data = std::get<String>(e.evaluate(a[3]));
	
//	std::cout << chr_data.size() << ":" << chr_data << "\n";
	for (int y = 0; y < 8; ++y){
		for (int x = 0; x < 8; ++x){
			auto d = chr_data[x+8*y];
			d = d - (d > '9' ? ('A' - '9' - 1) : 0) - '0';
			chr_resource.set_pixel(chr_id, x, y, d);
		}
	}
	regen_chr(resource_type);
}

void Visual::chrread_(const Args& a){
	auto a_ = a[1]; //copy so args are not modified
	//remove parens
	a_.erase(a_.begin());
	a_.erase(a_.end()-1);
	
	auto args = split(a_);
	
	auto resource_type = std::get<String>(e.evaluate(args[0]));
	int chr_id = std::get<Number>(e.evaluate(args[1]));
	auto& chr_resource = r.chr.at(resource_type);
	
	const char* digits = "0123456789ABCDEF";
	std::string chr_data{};
	for (int y = 0; y < 8; ++y){
		for (int x = 0; x < 8; ++x){
			chr_data += digits[chr_resource.get_pixel(chr_id, x, y)];
		}
	}
	
	e.assign(a[2], Token{chr_data, Type::Str});
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

void Visual::cls_(const Args&){
	c.cls({});
	p.get_console().reset();
}

void Visual::acls_(const Args&){
	//VISIBLE 1,1,1,1,1,1
	for (int i = 0; i < 6; i++)
		visible[i] = true;
	//ICONCLR
	//COLOR 0:CLS
	c.reset();
	p.get_console().reset();
	//GDRAWMD FALSE:GCLS (all):GCOLOR 0'(all GRP)
	g.reset();
	//SPCLR (all, both pages)
	s.reset();
	//TODO: rest of ACLS
	// BGOFS (all),0,0
	// BGCLR (all):BGCLIP 0,0,31,23
	b.reset();
	// COLINIT (all, both screens)
}

void Visual::load_(const Args& a){
	// LOAD <filename> [dialog]
	auto info = std::get<String>(e.evaluate(a[1]));
	auto type = info.substr(0,info.find(":"));
	auto name = info.substr(info.find(":")+1);
	
	type = r.normalize_type(type);
	r.load(type, name);
	if (std::find(r.chr_resources.begin(), r.chr_resources.end(), type) != r.chr_resources.end()){
		regen_chr(type);
	} else if (std::find(r.col_resources.begin(), r.col_resources.end(), type) != r.col_resources.end()) {
		regen_col();
	} else if (type == "MEM"){
		*std::get<String*>(e.vars.get_var_ptr("MEM$")) = r.mem.get_mem();
	}
}

void Visual::save_(const Args& a){
	auto info = std::get<String>(e.evaluate(a[1]));
	auto type = info.substr(0,info.find(":"));
	auto name = info.substr(info.find(":")+1);
	
	type = r.normalize_type(type);
	
	if (type == "MEM"){
		auto mem = *std::get<String*>(e.vars.get_var_ptr("MEM$"));
		r.mem.set_mem(mem);
		r.save(type, name);
	}
}


void Visual::update(){
	e.vars.write_sysvar("MAINCNTL", *maincntl+1);
	//do interpolation here as well (bg, sp)
	s.update();
	b.update();
}

//note: don't make this silly mistake
//https://en.sfml-dev.org/forums/index.php?topic=13526.0
void Visual::draw(sf::RenderWindow& w){
	//common
	bgsp_shader.setUniform("colors", col_tex);
	bgsp_shader.setUniform("texture", sf::Shader::CurrentTexture);
	sf::RenderStates rs;
	rs.shader = &bgsp_shader;
	//screens
	for (int sc = 0; sc < 2; ++sc){
		float col_l = 3.0f * sc;
		int chr_l = 6 * sc;
		//grp
		sf::Sprite grp;
		grp_tex.update(g.draw(sc).data());
		grp.setTexture(grp_tex);
		grp.setPosition(0,192*sc);
		grp.setColor(sf::Color(0));
		//grp prio=3
		if (g.get_prio(sc) == 3){
			bgsp_shader.setUniform("colbank", COL_GRP + col_l);
			w.draw(grp, rs);
		}
		//sprites prio=3
		rs.texture = &resource_tex[2 + chr_l];
		auto spr = s.draw(sc,3);
		bgsp_shader.setUniform("colbank", COL_SP + col_l);
		w.draw(spr, rs);
		//bg
		for (int l = 1; l >= 0; --l){
			bgsp_shader.setUniform("colbank", COL_BG + col_l);
			rs.texture = &resource_tex[1];
			auto& bg = b.draw(sc, l);
			auto pos = bg.getPosition();
			pos.x = (int)pos.x & 0x1ff;
			pos.y = (int)pos.y & 0x1ff;
			pos.y += 192*sc;
			if (pos.x > 0){
				bg.setPosition(pos.x - 64*8, pos.y);
				w.draw(bg, rs);
			}
			if (pos.y > 192*sc){
				bg.setPosition(pos.x, pos.y - 64*8);
				w.draw(bg, rs);
			}
			if (pos.x > 0 && pos.y > 0){
				bg.setPosition(pos.x - 64*8, pos.y - 64*8);
				w.draw(bg, rs);
			}
			bg.setPosition(pos);
			w.draw(bg, rs);
			//sprites prio=2, graphics prio=2
			if (l == 1){
				if (g.get_prio(sc) == 2){
					bgsp_shader.setUniform("colbank", COL_GRP + col_l);
					w.draw(grp, rs);
				}
				
				rs.texture = &resource_tex[2 + chr_l];
				spr = s.draw(sc,2);
				bgsp_shader.setUniform("colbank", COL_SP + col_l);
				w.draw(spr, rs);
			}
		}
		//grp prio=1
		if (g.get_prio(sc) == 1){
			bgsp_shader.setUniform("colbank", COL_GRP + col_l);
			w.draw(grp, rs);
		}
		//sprite prio=1
		rs.texture = &resource_tex[2 + chr_l];
		spr = s.draw(sc,1);
		bgsp_shader.setUniform("colbank", COL_SP + col_l);
		w.draw(spr, rs);
		//console
		if (sc == 0){
			rs.texture = &resource_tex[0 + chr_l];
			auto& con = c.draw();
			bgsp_shader.setUniform("colbank", COL_BG + col_l);
			w.draw(con, rs);
		} else {
			rs.texture = &resource_tex[0 + chr_l];
			auto& con = p.get_console().draw();
			con.setPosition(sf::Vector2f{0,192});
			bgsp_shader.setUniform("colbank", COL_BG + col_l);
			w.draw(con, rs);
		}
		//grp prio=0
		if (g.get_prio(sc) == 0){
			bgsp_shader.setUniform("colbank", COL_GRP + col_l);
			w.draw(grp, rs);
		}
		//sprite prio=0
		rs.texture = &resource_tex[2 + chr_l];
		spr = s.draw(sc,0);
		bgsp_shader.setUniform("colbank", COL_SP + col_l);
		w.draw(spr, rs);
	}
	
	if (p.panel_on()){
		auto& pnl = p.draw_panel();
		bgsp_shader.setUniform("colbank", COL_BG + 3.0f);
		rs.texture = &resource_tex[11];
		w.draw(pnl, rs);
		
		auto keysp = p.draw_keyboard();
		bgsp_shader.setUniform("colbank", COL_SP + 3.0f);
		rs.texture = &resource_tex[10];
		w.draw(keysp, rs);
		
		rs.texture = &resource_tex[6];
		auto& con = p.draw_funckeys();
		bgsp_shader.setUniform("colbank", COL_BG + 3.0f);
		w.draw(con, rs);
	}
}
