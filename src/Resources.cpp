#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iterator>

#include <iostream>

#include "Resources.hpp"
#include "Vars.hpp"
#include "Evaluator.hpp"
#include "FileLoader.hpp"

void Resources::load_program(std::string name){
	auto fs = get_filestream(name);
	read_n(fs, prg_info.data, 60);
	read_n(fs, prg.data, prg_info.get_prg_size());
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
		col.insert({c, COL()});
		col.at(c).load("resources/graphics/"+c.substr(0,4)+".PTC");
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
	
	auto memfs = get_filestream("resources/misc/MCHRENC.PTC");
	read_n(memfs, mem.data_enc, 48);
	read_n(memfs, mem.data_enc, MEM::SIZE);
	mem.generate_encoding();
}

std::string Resources::normalize_type(std::string type){
	if (type == "MEM")
		return type;
	
	if (type.size()==3){
		type += "0U"; //assumed
	}
	if (type.size()==4){
		if(type[3] == 'L' || type[3] == 'U'){
			return type.substr(0,3) + "0" + type[3];
		}
		auto t = type.substr(0,3);
		if (t == "BGF" || t == "BGU" || t == "BGD")
			type += "U";
		else if (t == "SPS")
			type += "L";
	}
	return type;
}

void Resources::load(std::string type, std::string filename){
	if (std::find(chr_resources.begin(), chr_resources.end(), type) != chr_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, chr.at(type).data, 48); //dummy read to skip header
		read_n(fs, chr.at(type).data, CHR::SIZE);		
	} else if (std::find(col_resources.begin(), col_resources.end(), type) != col_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, col.at(type).data, 48); //dummy read to skip header
		read_n(fs, col.at(type).data, COL::SIZE);		
	} else if (std::find(grp_resources.begin(), grp_resources.end(), type) != grp_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, grp.at(type).data, 48); //dummy read to skip header
		read_n(fs, grp.at(type).data, GRP::SIZE);		
	} else if (std::find(scr_resources.begin(), scr_resources.end(), type) != scr_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, scr.at(type).data, 48); //dummy read to skip header
		read_n(fs, scr.at(type).data, SCR::SIZE);		
	} else if (type == "MEM"){
		try {
			auto fs = get_filestream("programs/"+filename+".PTC");
			read_n(fs, mem.data, 48); //dummy read to skip header
			read_n(fs, mem.data, MEM::SIZE);
		} catch (std::runtime_error& e){
			std::cout << "Failed to load: " << e.what() << "\n";
			mem.data.resize(MEM::SIZE);
			std::fill(mem.data.begin(), mem.data.end(), 0);
		}
	}
}

void Resources::save(std::string type, std::string filename){
	Header header{};
	header.set_filename(filename);
	header.set_type(type);
	
	if (type == "MEM"){
		header.set_size(12 + MEM::SIZE);
		std::vector<unsigned char> data{mem.data};
		header.set_md5(data);
		
		auto fs = write_filestream("programs/"+filename+".PTC");
		write_n(fs, header.data);
		write_n(fs, data);
	}
}

