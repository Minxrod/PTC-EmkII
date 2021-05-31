#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iterator>

#include <iostream>

#include "Resources.h"
#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"

void Resources::load_program(std::string name){
	auto fs = get_filestream(name);
	read_n(fs, prg_info.data, 60);
	read_n(fs, prg.data, prg_info.get_size());
}

void Resources::load_default(){
	for (auto r : chr_resources){
		auto fs = get_filestream("resources/graphics/"+r.substr(0,4)+".PTC");
		chr.insert(std::pair(r, CHR()));
		read_n(fs, chr.at(r).data, 48); //dummy read to skip header
		read_n(fs, chr.at(r).data, CHR::SIZE);
	}
	//Keyboard sprites
	{
		auto fs = get_filestream("resources/ui/partsSPDK.NCGR");
		std::vector<unsigned char> dummy{};
		read_n(fs, dummy, 48+4*(256*64/2)); //dummy read to skip header, SPD0-3
		for (int i = 0; i < 4; ++i){
			chr.insert(std::pair("SPK" + std::to_string(i), CHR()));
			read_n(fs, chr.at("SPK" + std::to_string(i)).data, CHR::SIZE);
		}
	}
	
	for (auto c : col_resources){
		auto fs = get_filestream("resources/graphics/"+c.substr(0,4)+".PTC");
		col.insert(std::pair(c, COL()));
		read_n(fs, col.at(c).data, 48); //dummy read to skip header
		read_n(fs, col.at(c).data, COL::SIZE);
	}
	
	for (auto g : grp_resources){
		grp.insert(std::pair(g, GRP()));
		grp.at(g).data = std::vector<unsigned char>(GRP::SIZE, 0);
	}
	
	for (auto s : scr_resources){
		scr.insert(std::pair(s, SCR()));
		scr.at(s).data = std::vector<unsigned char>(SCR::SIZE, 0);
	}
	
	//Panel SCRs
	scr.insert(std::pair("PNL", SCR()));
	auto pnlfs = get_filestream("resources/ui/pnlPANEL.NSCR");
	read_n(pnlfs, scr.at("PNL").data, 36); //dummy read to skip header
	read_n(pnlfs, scr.at("PNL").data, 2*32*24);
	
	scr.insert(std::pair("KEY", SCR()));
	auto keyfs = get_filestream("resources/ui/pnlKEY.NSCR");
	read_n(keyfs, scr.at("KEY").data, 36); //dummy read to skip header
	read_n(keyfs, scr.at("KEY").data, 2*32*24);
}

