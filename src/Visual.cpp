#include "Visual.hpp"
#include "Vars.hpp"

#include <thread>

const float COL_BG = 0.1;
const float COL_SP = 1.1;
const float COL_GRP = 2.1;
const float COL_BG_L = 3.1;
const float COL_SP_L = 4.1;

/// Enum used for naming textures in resource_tex
enum Tex {
	BGF_U = 0, BGU_U, SPU, SPS_U, UNUSED_1, BGD_U,
	BGF_L, BGU_L, UNUSED_2, SPS_L, SPD, BGD_L
};

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
		throw std::runtime_error{"Error: Shaders are unavailable"};
	} else {
		if (!bgsp_shader.loadFromFile("resources/shaders/bgsp.frag", sf::Shader::Fragment)){
			std::cout << "Error: Shader bgsp.frag could not be loaded";
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
		cmd_map("COLSET"_TC, getfunc(this, &Visual::colset_)),
		cmd_map("COLREAD"_TC, getfunc(this, &Visual::colread_)),
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

/// PTC command to set the visible status of different components.
/// @note This function is implemented, but the results are currently ignored.
/// 
/// Format: 
/// * `VISIBLE con,pnl,bg0,bg1,sp,grp`
/// 
/// Arguments:
/// * con: true=Console is visible
/// * pnl: true=Panel is visible
/// * bg0: true=BG layer 0 is visible
/// * bg1: true=BG layer 1 is visible
/// * sp: true=Sprites are visible
/// * grp: true=Graphics page is visible
/// 
/// @param a Arguments
void Visual::visible_(const Args& a){
	for (int i = 0; i < 6; i++)
		visible[i] = (bool)(int)std::get<Number>(e.evaluate(a[i+1]));
}

/// PTC command to wait for vertical synchronization.
/// 
/// Format: 
/// * `VSYNC frames`
/// 
/// Arguments:
/// * frames: Number of frames to wait for
/// 
/// @param a Arguments
void Visual::vsync_(const Args& a){
	int frames = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	int wait_until = (int)*maincntl + frames;
	
	while (*maincntl < wait_until){
		//?? idk;
		//https://stackoverflow.com/questions/7083482/how-to-prevent-gcc-from-optimizing-out-a-busy-wait-loop
		std::this_thread::yield();
	}
}

/// PTC command to restore character(s) to their default state.
/// 
/// Format: 
/// * `CHRINIT resource`
/// 
/// Arguments:
/// * resource: Resource to reset
/// 
/// @param a Arguments
void Visual::chrinit_(const Args& a){
	//CHRINIT resource
	String res = std::get<String>(e.evaluate(a[1]));
	res = r.normalize_type(res, b.get_page(), s.get_page());
	std::string path = "resources/graphics/"+res.substr(0,4)+".PTC";
	load_default(r.chr.at(res), path);
	//needs to regen textures
	regen_chr(res);
}

/// PTC command to set new character data.
/// 
/// Format: 
/// * `CHRSET resource,chr,data`
/// 
/// Arguments:
/// * resource: Bank to set data for
/// * chr: Character code [0-255]
/// * data: Data string (64 hex digits)
/// 
/// @param a Arguments
void Visual::chrset_(const Args& a){
	auto resource_type = std::get<String>(e.evaluate(a[1]));
	int chr_id = std::get<Number>(e.evaluate(a[2]));
	resource_type = r.normalize_type(resource_type, b.get_page(), s.get_page());
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

/// PTC command to read character data into a string.
/// 
/// Format: 
/// * `CHRREAD(resource,chr),data`
/// 
/// Arguments:
/// * resource: Resource bank to read from
/// * chr: Character code [0-255]
/// * data: Variable to write character data to
/// 
/// @param a Arguments
void Visual::chrread_(const Args& a){
	auto a_ = a[1]; //copy so args are not modified
	//remove parens
	a_.erase(a_.begin());
	a_.erase(a_.end()-1);
	
	auto args = split(a_);
	
	auto resource_type = std::get<String>(e.evaluate(args[0]));
	int chr_id = std::get<Number>(e.evaluate(args[1]));
	resource_type = r.normalize_type(resource_type);
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

/// PTC command to reset the colors to their default state.
/// 
/// Format: 
/// * `COLINIT resource[,color]`
/// 
/// Arguments:
/// * resource: Color resource to reset
/// * color: Color code to reset. If none, resets all colors.
/// 
/// @param a Arguments
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
		Number c = std::get<Number>(e.evaluate(a[2]));
		//needs to write specific color
		COL col{};
		col.load(path);
		
		r.col.at(res+"U").set_col(c, col.get_col(c));
		r.col.at(res+"L").set_col(c, col.get_col(c));
	}
}

/// PTC command to set a specific color.
/// 
/// Format: 
/// * `COLSET resource,color,data`
/// 
/// Arguments:
/// * resource: COL resource to write
/// * color: Color code [0-255]
/// * data: Color string "RRGGBB"
/// 
/// @param a Arguments
void Visual::colset_(const Args& a){
	//COLSET bank,color,data
	//Note: Affected by BGPAGE, SPPAGE, GPAGE?
	String res = std::get<String>(e.evaluate(a[1]));
	Number c = std::get<Number>(e.evaluate(a[2]));
	String data = std::get<String>(e.evaluate(a[3]));
	
	if (res == "BG"){res = "COL0";}
	if (res == "SP"){res = "COL1";}
	if (res == "GRP"){res = "COL2";}
	res = r.normalize_type(res, b.get_page(), s.get_page(), g.get_page());
	
	const std::string digits = "0123456789ABCDEF";
	
	int rd = ((digits.find(data[0]) << 4) + digits.find(data[1]));
	int g = ((digits.find(data[2]) << 4) + digits.find(data[3]));
	int b = ((digits.find(data[4]) << 4) + digits.find(data[5]));
	
	r.col.at(res).set_col(c, rd, g, b);
}

/// PTC command to read a color.
/// 
/// Format: 
/// * `COLREAD(resource,color),r,g,b`
/// 
/// Arguments:
/// * resource: COL resource to read
/// * color: Color code [0-255]
/// * r: Variable to write red component
/// * g: Variable to write green component
/// * b: Variable to write blue component
/// 
/// @param a Arguments
void Visual::colread_(const Args& a){
	// COLREAD (resource color) r g b
	auto a_ = a[1]; //copy so args are not modified
	//remove parens
	a_.erase(a_.begin());
	a_.erase(a_.end()-1);
	
	auto args = split(a_);
	
	auto res = std::get<String>(e.evaluate(args[0]));
	int col_id = std::get<Number>(e.evaluate(args[1]));
	
	if (res == "BG"){res = "COL0";}
	if (res == "SP"){res = "COL1";}
	if (res == "GRP"){res = "COL2";}
	res = r.normalize_type(res, b.get_page(), s.get_page(), g.get_page());
	
	auto& col_resource = r.col.at(res);
	
	auto red = col_resource.get_col_r(col_id);
	auto green = col_resource.get_col_g(col_id);
	auto blue = col_resource.get_col_b(col_id);
	
	e.assign(a[2], Token{std::to_string(red), Type::Num});
	e.assign(a[3], Token{std::to_string(green), Type::Num});
	e.assign(a[4], Token{std::to_string(blue), Type::Num});
}

/// PTC command to clear the console screens.
/// @note Clears both the upper screen console and lower screen console (`PNLSTR` output)
/// 
/// Format: 
/// * `CLS`
void Visual::cls_(const Args&){
	c.cls();
	p.get_console().cls();
}

/// PTC command to reset almost the entire graphics state.
/// @note Not yet fully implemented.
/// 
/// Format: 
/// * `ACLS`
void Visual::acls_(const Args&){
	//VISIBLE 1,1,1,1,1,1
	for (int i = 0; i < 6; i++)
		visible[i] = true;
	//TODO: ICONCLR
	//COLOR 0:CLS
	c.reset();
	p.get_console().reset();
	//GDRAWMD FALSE:GCLS (all):GCOLOR 0'(all GRP)
	g.reset();
	//SPCLR (all, both pages)
	s.reset();
	// BGOFS (all),0,0
	// BGCLR (all):BGCLIP 0,0,31,23
	b.reset();
	//TODO: rest of ACLS
	// COLINIT (all, both screens)
}

/// PTC command to load from a file.
/// @note Dialog is not implemented, so the silent flag is currently ignored.
/// 
/// Format: 
/// * `LOAD filename[,silent]`
/// 
/// Arguments:
/// * filename: File type and name string
/// * silent: true=Do not show load dialog
/// 
/// @param a Arguments
void Visual::load_(const Args& a){
	// LOAD <filename> [dialog]
	auto info = std::get<String>(e.evaluate(a[1]));
	auto type = info.substr(0,info.find(":"));
	auto name = info.substr(info.find(":")+1);
	
	type = r.normalize_type(type, b.get_page(), s.get_page());
	try {
		r.load(type, name);
		if (std::find(r.chr_resources.begin(), r.chr_resources.end(), type) != r.chr_resources.end()){
			regen_chr(type);
		} else if (std::find(r.col_resources.begin(), r.col_resources.end(), type) != r.col_resources.end()) {
			regen_col();
		} else if (type == "MEM"){
			*std::get<String*>(e.vars.get_var_ptr("MEM$")) = r.mem.get_mem();
		} else if (type.substr(0,3) == "GRP"){
			g.regen_grp(type[3]-'0');
		}
		e.vars.write_sysvar("RESULT",1.0);
	} catch (const std::runtime_error& ex){
		std::cout << ex.what();
		e.vars.write_sysvar("RESULT",0.0);
	}
}

/// PTC command to save to a file.
/// @warning Only MEM save is currently implemented.
/// 
/// Format: 
/// * `SAVE filename`
/// 
/// Arguments:
/// * filename: File type and name string.
/// 
/// @param a Arguments
void Visual::save_(const Args& a){
	auto info = std::get<String>(e.evaluate(a[1]));
	auto type = info.substr(0,info.find(":"));
	auto name = info.substr(info.find(":")+1);
	
	type = r.normalize_type(type, b.get_page(), s.get_page());
	
	if (type == "MEM"){
		auto mem = *std::get<String*>(e.vars.get_var_ptr("MEM$"));
		r.mem.set_mem(mem); //mem needs to set from variable
		r.save(type, name);
	} else {
		r.save(type, name); //every other type is already stored in resources
	}
}

void Visual::update(){
	//https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
	//https://en.cppreference.com/w/cpp/chrono
	auto time_t = std::time(0);
	std::tm* tm = std::localtime(&time_t);
	char date[10+1];//"yyyy/mm/dd\0";
	char time[8+1]; //"hh:mm:ss\0";
	strftime(date, 11, "%Y/%m/%d", tm);
	strftime(time, 9, "%H:%M:%S", tm);
	//Sets some system variables
	e.vars.write_sysvar("DATE$", std::string(date));
	e.vars.write_sysvar("TIME$", std::string(time));

	if (*maincntl > 524287){
		e.vars.write_sysvar("MAINCNTH", *maincnth+1);
		e.vars.write_sysvar("MAINCNTL", 0.0);
	} else {
		e.vars.write_sysvar("MAINCNTL", *maincntl+1);
	}
	//do interpolation here as well (bg, sp)
	s.update();
	b.update();
}

sf::RenderStates Visual::set_state(int chr, float col){
	sf::RenderStates rs;
	rs.shader = &bgsp_shader;
	rs.texture = &resource_tex[chr];
	bgsp_shader.setUniform("colbank", col);
	return rs;
}

//note: don't make this silly mistake
//https://en.sfml-dev.org/forums/index.php?topic=13526.0
void Visual::draw(sf::RenderWindow& w){
	//common
	bgsp_shader.setUniform("colors", col_tex);
	bgsp_shader.setUniform("texture", sf::Shader::CurrentTexture);
	//screens
	for (int sc = 0; sc < 2; ++sc){
		float col_l = 3.0f * sc;
		int chr_l = 6 * sc;
		// state functions
		auto grp_state = [this, col_l](){
			return set_state(Tex::BGU_U, COL_GRP + col_l);
		};
		auto sprite_state = [this, sc, col_l](){
			return set_state(sc ? Tex::SPS_L : Tex::SPU, COL_SP + col_l);
		};
		//grp
		sf::Sprite grp;
		grp_tex.update(g.draw(sc).data());
		grp.setTexture(grp_tex);
		grp.setPosition(0,192*sc);
		grp.setColor(sf::Color(0));
		//grp prio=3
		if (g.get_prio(sc) == 3){
//			bgsp_shader.setUniform("colbank", COL_GRP + col_l);
			w.draw(grp, grp_state());
		}
		//sprites prio=3
		w.draw(s.draw(sc,3), sprite_state());
		//bg
		for (int l = 1; l >= 0; --l){
			auto state = set_state(Tex::BGU_U + chr_l, COL_BG);
//			bgsp_shader.setUniform("colbank", COL_BG + col_l);
//			rs.texture = &resource_tex[1];
			auto& bg = b.draw(sc, l);
			auto pos = bg.getPosition();
			pos.x = (int)pos.x & 0x1ff;
			pos.y = (int)pos.y & 0x1ff;
			pos.y += 192*sc;
			if (pos.x > 0){
				bg.setPosition(pos.x - 64*8, pos.y);
				w.draw(bg, state);
			}
			if (pos.y > 192*sc){
				bg.setPosition(pos.x, pos.y - 64*8);
				w.draw(bg, state);
			}
			if (pos.x > 0 && pos.y > 0){
				bg.setPosition(pos.x - 64*8, pos.y - 64*8);
				w.draw(bg, state);
			}
			bg.setPosition(pos);
			w.draw(bg, state);
			
			// grp prio=2,1
			if (g.get_prio(sc) == 1+l){
				w.draw(grp, grp_state());
			}
			// sprite prio=2,1
			w.draw(s.draw(sc,1+l), sprite_state());
		}
		
		//console + panel console
		auto console_state = set_state(BGF_U + chr_l, COL_BG + col_l);
		auto& console = sc ? p.get_console().draw() : c.draw();
		if (sc){
			console.setPosition(sf::Vector2f{0,192});
		}
		w.draw(console, console_state);
		//grp prio=0
		if (g.get_prio(sc) == 0){
			w.draw(grp, grp_state());
		}
		//sprite prio=0
		w.draw(s.draw(sc,0), sprite_state());
	}
	
	if (p.panel_on()){
		w.draw(p.draw_panel(), set_state(Tex::BGD_L, COL_BG_L));
		w.draw(p.draw_keyboard(), set_state(Tex::SPD, COL_SP_L));
		w.draw(p.draw_funckeys(), set_state(Tex::BGF_L, COL_BG_L));
	}
	// icon probably draws over EVERYTHING so it's at the end
	w.draw(p.draw_icon(), set_state(Tex::SPD, COL_SP_L));
}
