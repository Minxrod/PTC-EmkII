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
	std::vector<std::string> chr_resources{
		"BGU0L", "BGU1L", "BGU2L", "BGU3L",
		"BGU0U", "BGU1U", "BGU2U", "BGU3U",
		"BGF0U",
		"BGF0L",
		"BGD0U", "BGD1U",
		"BGD0L", "BGD1L",
		"SPS0L", "SPS1L",
		"SPS0U", "SPS1U",
		"SPU0", "SPU1", "SPU2", "SPU3", "SPU4", "SPU5", "SPU6", "SPU7",
		"SPD0", "SPD1", "SPD2", "SPD3"
	};
	
	for (auto r : chr_resources){
		auto fs = get_filestream("resources/graphics/"+r.substr(0,4)+".PTC");
		chr.insert(std::pair(r, CHR()));
		read_n(fs, chr.at(r).data, 48); //dummy read to skip header
		read_n(fs, chr.at(r).data, CHR::SIZE);
	}
	
	std::vector<std::string> col_resources{
		"COL0U", "COL1U", "COL2U",
		"COL0L", "COL1L", "COL2L",
	};
	
	for (auto c : col_resources){
		auto fs = get_filestream("resources/graphics/"+c.substr(0,4)+".PTC");
		col.insert(std::pair(c, COL()));
		read_n(fs, col.at(c).data, 48); //dummy read to skip header
		read_n(fs, col.at(c).data, COL::SIZE);
	}
	
	std::vector<std::string> grp_resources{
		"GRP0", "GRP1", "GRP2", "GRP3"
	};
	
	for (auto g : grp_resources){
		grp.insert(std::pair(g, GRP()));
		grp.at(g).data = std::vector<unsigned char>(GRP::SIZE, 0);
	}
}

